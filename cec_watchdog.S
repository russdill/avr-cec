/*
 * Copyright (C) 2016 Russ Dill <russ.dill@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */
#define __SFR_OFFSET 0

#include <avr/io.h>

#include "cec_spec.h"
#include "bitops.h"
#include "div.h"

#define __zero_reg__ r1

#define DIV_ROUND_UP(n, d)		(1 + ((n) - 1) / (d))


/* Ideally, we count to 256 */
#define TCNT1_PRESCALER_IDEAL	DIV_ROUND_UP(F_CPU * MAX_LATENCY_US, \
								256 * 1000000)

/* But we can only do powers of 2, so round up to the next one */
#define TCNT1_PRESCALER_VAL	(LOG2(TCNT1_PRESCALER_IDEAL*2-1)+1)
//#define TCNT1_PRESCALER		(1 << (TCNT1_PRESCALER_VAL - 1))
#define TCNT1_PRESCALER		64

/*
 * If we can't measure anything for this long, then the line could have
 * gone high and then low again in our "absence". We would falsely think
 * that it had been too long since the line had been released.
 */
#define MAX_LATENCY_US		(2 * (CEC_T7_EARLY_END - CEC_T6_LATE0))


#define US_TO_TICKS(n)		(F_CPU * (n) / (TCNT1_PRESCALER * 1000000))
#define US_TO_TICKS_UP(n)	DIV_ROUND_UP(F_CPU * (n), \
						TCNT1_PRESCALER * 1000000)

#define CEC_PIN		PINB
#define CEC_PBIN	PB0
#define CEC_PBOUT	PB1

	.section .bss.cec_watchdog, "aw", @nobits
cec_watchdog_last_output:
	.zero	1

	.section .text.cec_watchdog, "ax", @progbits
.global cec_watchdog_periodic
cec_watchdog_periodic:
	/* Make the critical section as short as possible */

	/* Last line state */
	lds r18, cec_watchdog_last_output

	/* New line state floating (high) */
	ldi r21, 1

	ldi r20, US_TO_TICKS(MAX_LATENCY_US)
	ldi r22, _BV(PSR1)
	ldi r23, _BV(OCF1A) | _BV(OCF1B)

	/* Return value */
	clr r24

	cli
	/* Move our latency expiration forward */
	in r18, TCNT1
	add r18, r20
	out OCR1B, r18

	in r19, TIFR
	sbrc r19, OCF1B
	/* We've been out too long, assume that we just floated the output */
	rjmp rising_edge

	/* Check if we are already driving the line low */
	sbic CEC_PIN, CEC_PBOUT
	rjmp done

	/* New last state is line low */
	clr r21

	/* Check if we have a rising edge */
	tst r18
	breq no_edge

	/* Rising edge, reset our timers */
rising_edge:
	out OCR1B, r20
	out GTCCR, r22
	out TCNT1, __zero_reg__
	out TIFR, r13
	rjmp done

no_edge:
	/*
	 * During the time after a release of the CEC line, we ignore it's
	 * state.
	 */
	sbrc r19, OCF1A
	rjmp done

	/*
	 * If it's been longer than that and the line is low, it's because
	 * someone else is driving the line low. Return 1.
	 */
	sbic CEC_PIN, CEC_PBIN
	inc r24

done:
	sei
	sts cec_watchdog_last_output, r21
	ret

.global cec_watchdog_init
cec_watchdog_init:
	ldi r18, US_TO_TICKS_UP(CEC_MAX_RISE_TIME)
	out OCR1A, r18
	ldi r18, US_TO_TICKS(MAX_LATENCY_US)
	out OCR1B, r18
	ldi r18, _BV(PSR1)
	out GTCCR, r18
	out TCCR1, TCNT1_PRESCALER_VAL
	ret
