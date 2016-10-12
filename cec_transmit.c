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
#include <string.h>
#include <avr/io.h>
#include <stdbool.h>

#include "cec_spec.h"

#ifndef CEC_USI
#include "time.h"
#endif
#include "bitops.h"

#define TRANSMIT_IDLE		0
#define TRANSMIT_FAILED		_BV(0)
#define TRANSMIT_PEND		_BV(1)
#define TRANSMIT_AGAIN		(TRANSMIT_PEND | _BV(0))
#define TRANSMIT_BIT_EOM	_BV(2)
#define TRANSMIT_ACK		_BV(3)
#define TRANSMIT_WAIT_FOR_ACK	_BV(4)

static void cec_transmit_halt_hw(void);

unsigned char transmit_buf[CEC_BUFFER_SIZE];
static unsigned char transmit_buf_end;
unsigned char transmit_buf_bit;
unsigned char transmit_buf_pos;
static unsigned char transmit_last_bit;

#ifdef CEC_ERR_STATS
static unsigned char transmit_state_buf[7];
#define transmit_state transmit_state_buf[0]
#else
#ifdef CEC_TRANSMIT_STATE_REG
register unsigned char transmit_state CEC_TRANSMIT_STATE_REG;
#else
static unsigned char transmit_state;
#endif
#endif

static unsigned char transmit_retries;

#if defined(CEC_USI) || defined(CEC_TRANSMIT_PWM)
#define CHECK_BIT_DELAY 1
#endif

#ifdef CEC_USI
#ifdef CEC_NEEDED_IDLE_FRAMES_REG
register unsigned char needed_idle_frames CEC_NEEDED_IDLE_FRAMES_REG;
#else
unsigned char needed_idle_frames;
#endif
#else
unsigned int needed_idle_time;
#endif

static void cec_transmit_abort(void);

/* An error was detected */
static void cec_transmit_on_error(unsigned char err)
{
	if (transmit_state > TRANSMIT_AGAIN) {
#ifdef CEC_ERR_STATS
		transmit_state_buf[err]++;
#endif
		/* Inform the hardware */
		cec_transmit_abort();
	}
}

/* Hardware is done with abort */
static void cec_transmit_finish_abort(void)
{
	if (++transmit_retries == CEC_XMIT_MAX_RETRANSMIT)
		/* No more retransmits left */
		transmit_state = TRANSMIT_FAILED;

	else {
		/* Perform a retransmit */
#ifdef CEC_USI
		needed_idle_frames = CEC_PREV_PERIOD_WAIT;
#else
		needed_idle_time = US_TO_JIFFIES_UP(CEC_PREV_PERIOD_WAIT
								* CEC_PERIOD);
#endif
		transmit_state = TRANSMIT_AGAIN;
	}
}

static void cec_transmit_halt(void)
{
	transmit_state = TRANSMIT_IDLE;
	cec_transmit_halt_hw();
}

/* Make sure the our own receive side is in sync with what we are sending */
static void cec_check_tx_bit(unsigned char bit)
{
	unsigned char delayed = transmit_last_bit;
#ifdef CHECK_BIT_DELAY
	delayed >>= CHECK_BIT_DELAY;
	delayed &= 1;
#endif
	if (bit != delayed)
		cec_transmit_on_error(CEC_ERR_ARB_LOST);

}

static void cec_transmit_receive_ack(bool ack)
{
	if (ack) {
		if (transmit_state == TRANSMIT_WAIT_FOR_ACK)
			transmit_state = TRANSMIT_IDLE;
	} else
		cec_transmit_on_error(CEC_ERR_NACK);
}

/* Hardware wants us to start the next message */
static void cec_transmit_start(void)
{
	if (transmit_state == TRANSMIT_PEND) {
		transmit_retries = 0;
#ifdef CEC_ERR_STATS
		memset(transmit_state_buf+1, 0, sizeof(transmit_state_buf)-1);
#endif
	}
	transmit_buf_bit = _BV(7);
	transmit_buf_pos = 0;
	transmit_state = TRANSMIT_BIT_EOM;
}

/* Hardware driver wants the next bit */
static bool cec_transmit_get_bit(void)
{
	bool bit;
	unsigned char buf_bit;

	buf_bit = transmit_buf_bit;

	/* Setup the next cycle */
	if (buf_bit) {
		/* Data bit */
		bit = transmit_buf[transmit_buf_pos] & buf_bit;
		buf_bit >>= 1;

	} else {
		bool is_last = transmit_buf_end == transmit_buf_pos;
		unsigned char state = transmit_state;

		if (state == TRANSMIT_BIT_EOM) {
			/*
			 * EOM indicator bit. We don't EOM ping messages.
			 * This makes the means we'll get our ack, but
			 * it will never be considered a valid message.
			 */
			bit = is_last && transmit_buf_end;
			state = TRANSMIT_ACK;
		} else {
			/* Ack bit, end of frame */
			bit = true;

			if (is_last)
				state = TRANSMIT_WAIT_FOR_ACK;
			else {
				buf_bit = _BV(7);
				transmit_buf_pos++;
				state = TRANSMIT_BIT_EOM;
			}

			/* Consider us the present initiator */
#ifdef CEC_USI
			needed_idle_frames = CEC_PRESENT_PERIOD_WAIT;
#else
			needed_idle_time = US_TO_JIFFIES_UP(CEC_PRESENT_PERIOD_WAIT
								* CEC_PERIOD);
#endif
		}
		transmit_state = state;
	}

#ifdef CHECK_BIT_DELAY
	transmit_last_bit = (transmit_last_bit << 1) | bit;
#else
	transmit_last_bit = bit;
#endif

	transmit_buf_bit = buf_bit;

	return bit;
}

static void cec_transmit_init(void)
{
#ifdef CEC_USI
	needed_idle_frames = CEC_NEW_PERIOD_WAIT;
#else
	needed_idle_time = US_TO_JIFFIES_UP(CEC_NEW_PERIOD_WAIT * CEC_PERIOD);
#endif

#ifdef CEC_TRANSMIT_STATE_REG
	transmit_state = TRANSMIT_IDLE;
#endif
	cec_transmit_init_hw();
}
