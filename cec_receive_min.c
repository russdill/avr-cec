/*
 * Soft CEC receive driver, needs a time delta passed to the periodic
 * function.
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

static unsigned int receive_frame_period;
static unsigned int receive_frame_timer;
static unsigned int receive_frame_ack_done;
static bool last_state;
static bool sample;

static void cec_receive_nack_frame(void)
{
	/* We lost sync and don't know where to nack, just blast the line */
	cec_receive_drive_low();

	/* Nack for a full frame */
	receive_frame_ack_done = US_TO_JIFFIES(10 * CEC_PERIOD);
	receive_frame_timer = 0;
}

static void cec_receive_periodic(unsigned int delta)
{
	bool state;

	/* Add, but cap at ~0xffff */
	asm(
		"add	%A0, %A1\n"
		"adc	%B0, %B1\n"
		"brcc	1f\n"
		"ldi	%B0, 0xff\n"
		"1:\n"
		: "=a"(receive_frame_timer)
		: "r"(delta), "0"(receive_frame_timer)
	);

	state = cec_input_state();
	if (sample && receive_frame_timer > US_TO_JIFFIES(CEC_T3)) {
		sample = false;
		if (receive_frame_timer > US_TO_JIFFIES_UP(CEC_T4))
			/* Latency failure */
			cec_receive_error(CEC_ERR_HW);
		else
			/* Sample our bit */
			cec_receive_bit(state);
	}

	if (receive_frame_timer > receive_frame_ack_done)
		/* Done acking/nacking */
		cec_receive_float();

	if (state == last_state) {
		if (receive_frame_timer > receive_frame_period + (unsigned short) US_TO_JIFFIES_UP(800))
			/* We've gone 600uS without an expected transition */
			cec_receive_error(CEC_ERR_NO_EOM);

	} else if (state) {
		/* Rising edge */
		if (receive_frame_timer > US_TO_JIFFIES(CEC_START_LOW_EARLY) &&
		    receive_frame_timer < US_TO_JIFFIES_UP(CEC_START_LOW_LATE)) {
			receive_frame_period = US_TO_JIFFIES_RND(CEC_START_HIGH_EARLY - 200);
			/* Start */
			cec_receive_start();
		}
	} else {
		/* Falling edge */
		if (cec_receive_flags) {
			if (receive_frame_timer < receive_frame_period)
				/* Error */
				cec_receive_error(CEC_ERR_LOW_DRIVE);
			else {
				if ((cec_receive_flags & CEC_RECV_DO_ACK) &&
				   !(cec_receive_flags & CEC_RECV_BITS_EOM)) {
					cec_receive_drive_low();
					receive_frame_ack_done =
						US_TO_JIFFIES(CEC_T5_EARLY0);
				}
				sample = true;
			}
		}
		receive_frame_period = US_TO_JIFFIES_RND(CEC_T7_EARLY_END - 50);
		receive_frame_timer = 0;
	}

	last_state = state;
}

static void cec_receive_halt_hw(void)
{
}

static void cec_receive_init(void)
{
}
