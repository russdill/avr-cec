/*
 * CEC transmit and receive with the USI hardware
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

#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/atomic.h>

#include "div.h"
#include "bitops.h"

#include "cec_spec.h"

#define SAMPLE_US	(CEC_PERIOD / 8ULL)

#include "time.h"

#define TCNT0_TOP	US_TO_JIFFIES_RND(SAMPLE_US)

enum {
	USI_RECV_IDLE,
	USI_RECV_START,
	USI_RECV_BITS,
};

enum {
	USI_XMIT_IDLE,
	USI_XMIT_START,
	USI_XMIT_BITS,
};

/*
 * The CEC bit period is 2.4ms, or 8 units of 300us. This makes it really
 * easy to transmit bit period. Since our CEC output is inverted, a one
 * pulls the line low, and a zero lets it float. The USI shifts out
 * starting with the MSB, so the bit patterns below match the output
 * waveform left to right.
 *
 * The start bit period is 4.5ms, which is 15 units of 300us, so we need
 * two bytes. The timing for the start bit is 100us off, but spec allows
 * for +/- 200us.
 */

#define CEC_PAT_START0	0x7f
#define CEC_PAT_START1	0xf8
#define CEC_PAT_1	0xc0
#define CEC_PAT_0	0xf8
#define CEC_PAT_IDLE	0x00

#define MIN_TICKS(n)	((n - 5) / (SAMPLE_US))
#define MAX_TICKS(n)	(1 + (n + 5) / (SAMPLE_US))

static void cec_usi_frame_hook(void);

#ifdef CEC_RECV_FRAME_TICK_REG
register unsigned char recv_frame_tick CEC_RECV_FRAME_TICK_REG;
#else
static unsigned char recv_frame_tick;
#endif

static unsigned char recv_frame;
static unsigned char min_frame_ticks;
static unsigned char max_frame_ticks;
static unsigned char idle_frames;
static unsigned char usi_recv_state;
static unsigned char usi_xmit_state;
static unsigned char float_ticks_max;
static unsigned char float_ticks_max_next;
static bool recv_last_bit;

#define FLAG1_CEC_USI_NACKING	0
#define FLAG1_CEC_USI_ACK_DONE	1

/* Process a 300uS time period (bit_state) */
static void cec_process_tick(bool bit_state)
{
	unsigned char recv_state = usi_recv_state;

	if (++recv_frame_tick == 0)
		recv_frame_tick = 255;

	if (recv_state == USI_RECV_BITS &&
			recv_frame_tick == CEC_NOM_SAMPLE / SAMPLE_US) {
		/* We are in the sample window */
		cec_receive_bit(bit_state);
		if (!cec_receive_flags)
			recv_state = USI_RECV_IDLE;
	}

	if (bit_state == recv_last_bit) {
		/* No transition */
		if (recv_frame_tick > max_frame_ticks) {
			/* Too much time passed with no transition */
			cec_receive_error(CEC_ERR_NO_EOM);
			recv_state = USI_RECV_IDLE;
		}

	} else if (!bit_state) {
		/* Falling edge */
		unsigned char frame = recv_frame + 8;

		if (frame == 10 * 8) {
			GPIOR1 &= ~_BV(FLAG1_CEC_USI_ACK_DONE);
			frame = 0;
		}

		if (recv_frame_tick < min_frame_ticks) {
			/* Frame period was too short */
			cec_receive_error(CEC_ERR_LOW_DRIVE);
			recv_state = USI_RECV_IDLE;
		} else if (recv_state == USI_RECV_START) {
			/* Successful start bit */
			cec_receive_start();
			GPIOR1 &= ~_BV(FLAG1_CEC_USI_ACK_DONE);
			frame = 0;
			recv_state = USI_RECV_BITS;
			min_frame_ticks = MIN_TICKS(CEC_T7_EARLY_END);
			max_frame_ticks = MAX_TICKS(CEC_T8_LATE_END);
		}

		recv_frame = frame;
		recv_frame_tick = 0;

	} else {
		/* Rising edge */
		if (recv_frame_tick >= MIN_TICKS(CEC_START_LOW_EARLY) &&
		    recv_frame_tick <= MAX_TICKS(CEC_START_LOW_LATE)) {
			/* The low period is a start condition */
			recv_state = USI_RECV_START;
			min_frame_ticks = MIN_TICKS(CEC_START_HIGH_EARLY);
			max_frame_ticks = MAX_TICKS(CEC_START_HIGH_LATE);
		}
	}

	usi_recv_state = recv_state;
	recv_last_bit = bit_state;
}

static void cec_transmit_abort(void)
{
	unsigned char sr;
	unsigned char dr;
	unsigned char bit = 0x7f;

	/*
	 * Called from either cec_transmit_on_error or
	 * cec_transmit_receive_ack.
	 */
	usi_xmit_state = USI_XMIT_IDLE;

	/*
	 * USIDR cotains valid input data, but we can't keep sending bad
	 * data. Stop sending data as soon as possible.
	 */
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		TCCR0B = 0;
		sr = (USISR & 7) - 8;
		dr = USIDR;
		do {
			dr &= bit;
			bit >>= 1;
		} while (++sr);
		USIDR = dr;
		TCCR0B = TCNT0_PRESCALER_VAL;
	}

	cec_transmit_finish_abort();
}

static void cec_receive_nack_frame(void)
{
	/* We lost sync and don't know where to nack, just blast the line */
	cec_receive_drive_low();
	GPIOR1 |= _BV(FLAG1_CEC_USI_NACKING);
}

#if CEC_MONITOR
static unsigned char cec_usi_next_bit(void)
{
	return CEC_PAT_IDLE;
}
#else
static unsigned char cec_usi_next_bit(void)
{
	bool bit;
	unsigned char ret;
	unsigned char state = usi_xmit_state;

	switch (state) {
	case USI_XMIT_IDLE:
		/*
		 * Start a transfer if it's pending and we have enough
		 * idle frames.
		 */
		if ((transmit_state & TRANSMIT_PEND) &&
				idle_frames >= needed_idle_frames) {
			cec_transmit_start();
			float_ticks_max_next = 8;
			state = USI_XMIT_START;
			ret = CEC_PAT_START0;
		} else
			ret = CEC_PAT_IDLE;
		break;

	case USI_XMIT_START:
		state = USI_XMIT_BITS;
		float_ticks_max_next = DIV_ROUND_UP(
			CEC_START_LOW + CEC_MAX_RISE_TIME, SAMPLE_US) - 8;
		ret = CEC_PAT_START1;
		break;

	default:
		bit = cec_transmit_get_bit();
		if (transmit_state == TRANSMIT_WAIT_FOR_ACK)
			state = USI_XMIT_IDLE;

		float_ticks_max_next = bit && recv_frame != 7 * 8 ?
			DIV_ROUND_UP(CEC_1 + CEC_MAX_RISE_TIME, SAMPLE_US) :
			DIV_ROUND_UP(CEC_0 + CEC_MAX_RISE_TIME, SAMPLE_US);

		ret = bit ? CEC_PAT_1 : CEC_PAT_0;
	}
	usi_xmit_state = state;
	return ret;
}
#endif

static void cec_transmit_periodic(unsigned int delta)
{
}

/*
 * Acking is a bit difficult because of the way we queue up output bits,
 * we find out pretty late that we need to send an ack so we need to
 * inject it.
 */
static void cec_receive_do_ack(signed char acks)
{
	unsigned char reg1 = TCNT0_TOP - 1;
	unsigned char pre = TCNT0_PRESCALER_VAL;

	/*
	 * We give ourselves at least one timer0 tick (64 cycles) to finish
	 * modification of USIDR. v-usb restricts us further to 52 cycles.
	 *
	 * In order to acheive this, we only enter the critical section if
	 * TCNT0 != TCNT0_TOP - 1. If TCNT0 is one below TCNT0_TOP, we wait
	 * for it to rollover, this can be up to 64 cycles (~4uS).
	 */

	asm(
"	in r23, %[sreg]\n"
"	cli\n"
"	out %[tccr0b], __zero_reg__\n"

	/*
	 * First we need to count how many ticks we've missed:
	 *
	 *                USISR=5
	 *          7   6   5   4   3   2   1   0
	 * USIDR: [ x | x | x | H | H | H | H | L ]
	 *
	 * So above, we've shifted in 5 bits of input. Only bit 0 is low
	 * so its been between 1 and 2 ticks since the line went low.
	 */

	/* Check how many bits have been shifted into USIDR (data register) */
"	in r21, %[usisr]\n"
"	andi r21, 7\n"

	/* None, less than one tick has passed */
"	breq ack_dr\n"

	/*
	 * Shift out a copy of SR until we find the first high bit. Decrement
	 * acks and the count of bits. If acks reaches zero, more than
	 * 1200uS has passed and we've missed any opportunity to ack. If
	 * out bit count reaches zero, it means that the previous high state
	 * is in the BR register, already processed and subtracted out of
	 * acks.
	 */
"	mov r22, r21\n"
"	in %[reg1], %[usidr]\n"
"1:	lsr %[reg1]\n"
"	brcc ack_dr\n"
"	dec %[acks]\n"
"	breq done\n"
"	dec r22\n"
"	brne 1b\n"

	/*
	 * Now we start acking, starting with the USIDR. The number of bits
	 * in the USIDR yet to be shifted out is 8 - USISR, the highest bit
	 * is transparent to the outside. We write up to acks ack bits. If
	 * acks reaches zero, we are done acking.
	 */

	/* Count from negative values to 0 */
"ack_dr:\n"
"	subi r21, 8\n"

	/* Our ack bits, we always have at least one */
"	ldi r22, 0x80\n"

"	rjmp 2f\n"
"1:	asr r22\n"	/* Use asr here instead of sec/ror */
"2:	dec %[acks]\n"
"	breq 1f\n"
"	inc r21\n"
"	brne 1b\n"

	/* Store our ack bits */
"1:	in %[reg1], %[usidr]\n"
"	or %[reg1], r22\n"
"	out %[usidr], %[reg1]\n"

	/*
	 * Critical section done, we should have quite a long time before
	 * USIBR is overwritten.
	 */
"	out %[sreg], r23\n"
"	out %[tccr0b], %[tcnt0_prescaler_val]\n"

	/* Check the state of our acks count */
"	tst %[acks]\n"
"	breq done\n"

	/*
	 * We have more bits left to ack. Store them in the USIBR (buffer
	 * register).
	 */

	/* We have at least one bit left to ack */
"	ldi r22, 0x80\n"
"	rjmp 2f\n"
"1:	asr r22\n"
"2:	dec %[acks]\n"
"	brne 1b\n"

"	out %[usibr], r22\n"

"done:\n"
"	out %[tccr0b], %[tcnt0_prescaler_val]\n"
"	out %[sreg], r23\n"
	:
	:	[usidr] "I"(_SFR_IO_ADDR(USIDR)),
		[usibr] "I"(_SFR_IO_ADDR(USIBR)),
		[usisr] "I"(_SFR_IO_ADDR(USISR)),
		[tcnt0] "I"(_SFR_IO_ADDR(TCNT0)),
		[tccr0b] "I"(_SFR_IO_ADDR(TCCR0B)),
		[sreg] "I"(_SFR_IO_ADDR(SREG)),
		[tcnt0_prescaler_val] "r"(pre),
		[acks] "r"(acks),
		[reg1] "r"(reg1)
	:	"r21", "r22", "r23"
	);
}

static void cec_receive_periodic(unsigned short delta)
{
	unsigned char buf;
	unsigned char bit;
	unsigned char tick;
	unsigned char frames;

	buf = USISR;

	if (buf & _BV(USIOIF))
		float_ticks_max = float_ticks_max_next;

	tick = buf & 0x7;
	if (!cec_input_state() && tick >= float_ticks_max)
		/* Line was driven by another host */
		cec_transmit_on_error(CEC_ERR_ARB_LOST);

	if (!(buf & _BV(USIOIF)))
		goto out;

	/* Provide a tick every 2.4ms */
	cec_usi_frame_hook();

	/* We nack for at least one full frame */
	if (!(GPIOR1 & _BV(FLAG1_CEC_USI_NACKING)))
		cec_receive_float();
	GPIOR1 &= ~_BV(FLAG1_CEC_USI_NACKING);

	/* Read in the new data */
	buf = USIBR;

	/* Count the number of concurrent idle frames */
	frames = idle_frames;
	if (buf)
		frames = 0;
	else if (!++frames)
		frames = 255;
	idle_frames = frames;

	for (bit = 0; bit < 8; bit++) {
		cec_process_tick(!(buf & 0x80));
		buf <<= 1;
	}

	/* Write out data */
	USIBR = cec_usi_next_bit();
	USISR |= 8;
	USISR |= _BV(USIOIF);

out:
	/* Handle outgoing acks */
	if (!(GPIOR1 & _BV(FLAG1_CEC_USI_ACK_DONE)) &&
		(cec_receive_flags & CEC_RECV_DO_ACK) && !cec_input_state()) {
		signed char acks = 0;
		unsigned char ticks = recv_frame_tick;
		ticks += USISR & 7;
		/* We want to ack for up to 5 ticks, or 1500uS */

		if (recv_frame == 9 * 8 && recv_frame_tick < 4) {
			/*
			 * We are already pretty late, maybe we can get
			 * something out.
		 	 */
			acks = 4 - recv_frame_tick;
		} else if (recv_frame == 8 * 8 &&
				ticks >= MAX_TICKS(CEC_T6_LATE0)) {
			acks = 5;
		} else
			return;

		GPIOR1 |= _BV(FLAG1_CEC_USI_ACK_DONE);
		cec_receive_do_ack(acks);
	}
}

static void cec_transmit_halt_hw(void)
{
	USIBR = 0;
	USIDR = 0;
}

static void cec_receive_halt_hw(void)
{
	cec_receive_float();
}

static void cec_transmit_init_hw(void)
{
}

static void cec_receive_init(void)
{
	USIBR = CEC_PAT_IDLE;
	USIDR = CEC_PAT_IDLE;
	USISR = _BV(USIOIF) | 8;

	/* Select Three-wire mode and Timer/Counter0 Compare Match clock */
	USICR = _BV(USIWM0) | _BV(USICS0);

	OCR0A = TCNT0_TOP;

	/* Enable clear to compare mode */
	TCCR0A = _BV(WGM01);

	/* Start timer */
	TCCR0B = TCNT0_PRESCALER_VAL;
}

