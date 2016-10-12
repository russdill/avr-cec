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
#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef CEC_USI
#include "time.h"
#endif

#define CEC_RECV_ACTIVE		_BV(0)
#define CEC_RECV_BCAST		_BV(1)
#if CEC_MONITOR
#define CEC_RECV_DO_ACK		0
#else
#define CEC_RECV_DO_ACK		_BV(2)
#endif
#define CEC_RECV_BITS_EOM	_BV(3)		/* State machine */
#define CEC_RECV_EOM		_BV(4)		/* EOM received */
#define CEC_RECV_IGNORE		_BV(5)

#define CEC_RECV_OVERRUN	CEC_STATUS_OVERRUN	/* _BV(6) */
#define CEC_RECV_NACKED		CEC_STATUS_NACK		/* _BV(7) */

#ifndef CEC_RECEIVE_BUF_HDR
#define CEC_RECEIVE_BUF_HDR 0
#endif

static void cec_receive_halt_hw(void);

/*
 * Output buffer. CEC_RECEIVE_BUF_HDR lets the app put bytes at the head
 * of the that we don't touch. This can allow the user application to
 * easily send out the buffer with an attached header.
 */
unsigned char cec_receive_buf[CEC_BUFFER_SIZE+CEC_RECEIVE_BUF_HDR+1];

/* Internal state */
static unsigned char cec_receive_byte;
static unsigned char cec_receive_pos;
#ifdef CEC_RECEIVE_FLAGS_REG
register unsigned char cec_receive_flags CEC_RECEIVE_FLAGS_REG;
#else
static unsigned char cec_receive_flags;
#endif

/* Callbacks or implementors */
static void cec_receive_nack_frame(void);

static void cec_receive_error(unsigned char err)
{
	if (!cec_receive_flags)
		/* Not doing anything, don't both */
		return;

	if (cec_receive_flags & CEC_RECV_BCAST)
		/* Need to nack */
		cec_receive_nack_frame();

	cec_receive_flags = 0;

	/* Tell the transmit side something went wrong */
	cec_transmit_on_error(err);
}

static void cec_receive_halt(void)
{
	cec_receive_buf[CEC_RECEIVE_BUF_HDR] = 0;
	cec_receive_halt_hw();
}

/* Underlying hardware got us a start frame */
static void cec_receive_start(void)
{
	/* Reset our receive buf */
	cec_receive_pos = 0;
	cec_receive_byte = _BV(0);
	cec_receive_flags = CEC_RECV_ACTIVE | CEC_RECV_BITS_EOM;

	/*
	 * Consider someone else present initiator, if we are
	 * transmitting, this will get reset.
	 */
#if !CEC_MONITOR
#ifdef CEC_USI
	needed_idle_frames = CEC_NEW_PERIOD_WAIT;
#else
	needed_idle_time = US_TO_JIFFIES_UP(CEC_NEW_PERIOD_WAIT * CEC_PERIOD);
#endif
#endif

	/*
	 * We want to make sure we aren't getting a start condition during
	 * active TX, but as TX may be a cycle or two ahead of RX, it's a
	 * bit complex to determine. Just ignore it and hope that things
	 * get screwed up worse down the line anyway.
	 */
}

/* Underlying hardware got us a new bit */
static void cec_receive_bit(bool bit)
{
	unsigned char flags = cec_receive_flags;
	unsigned char receive_pos = cec_receive_pos;

	if (flags & CEC_RECV_BITS_EOM) {
		/* We are receiving either the data byte, or the EOM */

		/* Make sure we match tx */
		cec_check_tx_bit(bit);
		unsigned char receive_byte = cec_receive_byte;

		if (receive_byte) {
			bool done;
			/* Data */

			/*
			 * We preload the receive_byte with a bit so that when
			 * that bit gets shifted out, we know we are done. Can
			 * be rol in asm.
			 */
			done = receive_byte & _BV(7);
			receive_byte = (receive_byte << 1) | bit;

			if (done) {
				bool nack = false;
				if (!receive_pos) {
					/*
					 * First byte, we now have the target
					 * address
					 */
					unsigned char addr = receive_byte & 0xf;

					if (addr == CEC_ADDR_BROADCAST)
						flags |= CEC_RECV_BCAST;
					else if (cec_addr_match(addr))
						flags |= CEC_RECV_DO_ACK;

					/*
					 * There is already a message pending,
					 * don't ack anything new.
					 */
					if (cec_receive_buf[CEC_RECEIVE_BUF_HDR]) {
						nack = true;
						flags |= CEC_RECV_IGNORE;
					}

				} else if (receive_pos == CEC_BUFFER_SIZE) {
					/*
					 * Buffer is now full, don't store
					 * further bytes and stop acking.
					 */
					flags |= CEC_RECV_OVERRUN;
					nack = true;
				}

				if (nack) {
					if (flags & CEC_RECV_BCAST)
						/* Broadcast: We need to nack */
						flags |= CEC_RECV_DO_ACK;
					else
						/* Follower: We can't ack */
						flags &= ~CEC_RECV_DO_ACK;
				} else if (!(flags & (CEC_RECV_IGNORE|CEC_RECV_OVERRUN)))
					cec_receive_buf[receive_pos + 1 + CEC_RECEIVE_BUF_HDR] = receive_byte;
				cec_receive_pos = receive_pos + 1;
				receive_byte = 0;
			}
			cec_receive_byte = receive_byte;
		} else {
			/* EOM */
			if (bit)
				flags |= CEC_RECV_EOM;

			/* Move to next state */
			flags &= ~CEC_RECV_BITS_EOM;
		}

	} else {
		/* Ack/nak bit */

		/* Ack reverses polarity on BCAST */
		if (!(flags & CEC_RECV_BCAST))
			bit = !bit;

		/* Make sure we match tx */
		cec_transmit_receive_ack(bit);

		if (!bit)
			flags |= CEC_RECV_NACKED;

		if (!bit || (flags & CEC_RECV_EOM)) {
			/* We are done */

			if (!(flags & CEC_RECV_IGNORE)) {
				cec_receive_buf[CEC_RECEIVE_BUF_HDR] =
					receive_pos | (flags & (
					CEC_STATUS_NACK | CEC_STATUS_OVERRUN));
			}

			/* Ignore remainder of message (if any) */
			flags = 0;

		} else {
			/* Next frame */
			cec_receive_byte = _BV(0);
			flags |= CEC_RECV_BITS_EOM;
		}
	}

	cec_receive_flags = flags;
}

