/*
 * CEC frames look a lot like PWM cycles. The period of each frame is the
 * same, but have different duty cycles. For the longer start frame
 * period, we just change the PWM period.
 *
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

#include <avr/interrupt.h>

#include <util/atomic.h>

#include "div.h"
#include "bitops.h"

#define TCNT1_PRESCALER_IDEAL   DIV_ROUND_UP((unsigned long long) F_CPU * \
			CEC_START_HIGH, 256000000)

#define TCNT1_PRESCALER_VAL	(LOG2(TCNT1_PRESCALER_IDEAL - 1) + 2)
#define TCNT1_PRESCALER		(1 << ((TCNT1_PRESCALER_VAL) - 1))

#define S_TO_CEC_JIFFIES(n)		((unsigned long) \
				(0.5 + (float) (n) * F_CPU / (TCNT1_PRESCALER)))
#define NS_TO_CEC_JIFFIES(n)	S_TO_CEC_JIFFIES((n) / 1000000000.0)
#define US_TO_CEC_JIFFIES(n)	S_TO_CEC_JIFFIES((n) / 1000000.0)
#define MS_TO_CEC_JIFFIES(n)	S_TO_CEC_JIFFIES((n) / 1000.0)

#define CEC_DATA_PERIOD		(US_TO_CEC_JIFFIES(CEC_PERIOD) - 1)
#define CEC_0_TICKS		(US_TO_CEC_JIFFIES(CEC_0) - 1)
#define CEC_1_TICKS		(US_TO_CEC_JIFFIES(CEC_1) - 1)

#define CEC_START_PERIOD	(US_TO_CEC_JIFFIES(CEC_START_HIGH) - 1)
#define CEC_START_TICKS		(US_TO_CEC_JIFFIES(CEC_START_LOW) - 1)

enum {
	XMIT_IDLE,
	XMIT_START,
	XMIT_RUNNING,
	XMIT_END1,
	XMIT_END2,
	XMIT_DONE,
};

static unsigned int transmit_high_timer;
static unsigned char xmit_state;

static void cec_transmit_abort(void) __attribute__((unused));
static void cec_transmit_abort(void)
{
	/* Stop the clock */
	TCCR1 = 0;

	/* Force the line low */
	GTCCR = _BV(COM1B1) | _BV(FOC1B);

	cec_transmit_finish_abort();

	xmit_state = XMIT_IDLE;
}

static void xmit_pwm_periodic(void)
{
	bool bit;

	if (!(TIFR & _BV(TOV1)))
		/* Not ready to load next frame yet */
		return;

	TIFR |= _BV(TOV1);

	switch (xmit_state) {
	case XMIT_RUNNING:
		if (!cec_receive_flags) {
			/* Receive not tracking us... */
			cec_transmit_on_error(CEC_ERR_ARB_LOST);
			break;
		}

		OCR1C = CEC_DATA_PERIOD;

	case XMIT_START:
		bit = cec_transmit_get_bit();
		OCR1B = bit ? CEC_1_TICKS : CEC_0_TICKS;
		if (transmit_state == TRANSMIT_WAIT_FOR_ACK)
			xmit_state = XMIT_END1;
		else
			xmit_state = XMIT_RUNNING;
		break;

	case XMIT_END1:
		/* Leave the line low (cec high) after this cycle */
		OCR1B = 0;
		xmit_state++;
		break;

	case XMIT_END2:
		/* Stop the clock */
		TCCR1 = 0;
		xmit_state = XMIT_IDLE;
		break;
	}
}

static void xmit_start(void)
{
	/* Have the interrupt fire after 1 timer clock cycle */
	TCNT1 = CEC_START_PERIOD - 1;
	OCR1C = CEC_START_PERIOD;

	/*
	 * Pulse Width Modulator B Enable, OC1x cleared on compare match.
	 * Set when TCNT1 = $00.
	 */
	GTCCR = _BV(PWM1B) | _BV(COM1B0);

	/* Load up the start bit for after the OCR1C match occurs */
	OCR1B = CEC_START_TICKS;

	/* Update state */
	xmit_state++;
	cec_transmit_start();

	/* Start the clock */
	TCCR1 = TCNT1_PRESCALER_VAL;
}

static void cec_transmit_periodic(unsigned int delta)
{
	if (xmit_state != XMIT_IDLE)
		xmit_pwm_periodic();

	if (!cec_input_state()) {
		transmit_high_timer = 0;
		return;
	}

	/* Add, but cap at ~0xffff */
	asm(
		"add	%A0, %A1\n"
		"adc	%B0, %B1\n"
		"brcc	1f\n"
		"ldi	%B0, 0xff\n"
		"1:\n"
		: "=a"(transmit_high_timer)
		: "r"(delta), "0"(transmit_high_timer)
	);

	if (transmit_high_timer < needed_idle_time)
		return;

	if (xmit_state == XMIT_IDLE) {
		if (transmit_state & TRANSMIT_PEND)
			xmit_start();
	}
}

static void cec_transmit_init_hw(void)
{
	/* Force the line low */
	GTCCR = _BV(COM1B1) | _BV(FOC1B);
}

static void cec_transmit_halt_hw(void)
{
}
