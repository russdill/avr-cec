/*
 * This is the traditional address assignment. We find an available logical
 * address based on our device type.
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

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "cec.h"
#include "cec_spec.h"

#ifdef CEC_LOGICAL_ADDRESS_REG
register unsigned char logical_address CEC_LOGICAL_ADDRESS_REG;
#else
static unsigned char logical_address;
#endif

static unsigned char cec_dev_idx;

/* Static table of which addresses are available given a device type */
PROGMEM static const unsigned char cec_dev_addrs[] = {
#ifndef CEC_DEV_TYPE
#error "CEC_DEV_TYPE not defined"
#elif CEC_DEV_TYPE == CEC_DEV_TV
	CEC_DEV_TV_ADDRS
#elif CEC_DEV_TYPE == CEC_DEV_RECORDING_DEVICE
	CEC_DEV_RECORDING_DEVICE_ADDRS
#elif CEC_DEV_TYPE == CEC_DEV_TUNER
	CEC_DEV_TUNER_ADDRS
#elif CEC_DEV_TYPE == CEC_DEV_PLAYBACK_DEVICE
	CEC_DEV_PLAYBACK_DEVICE_ADDRS
#elif CEC_DEV_TYPE == CEC_DEV_AUDIO_SYSTEM
	CEC_DEV_AUDIO_SYSTEM_ADDRS
#elif CEC_DEV_TYPE == CEC_DEV_SWITCH
	CEC_DEV_SWITCH_ADDRS
#elif CEC_DEV_TYPE == CEC_DEV_VIDEO_PROCESSOR
	CEC_DEV_VIDEO_PROCESSOR_ADDRS
#else
#error "Unknown CEC_DEV_TYPE"
#endif
};

/* source is always our assigned logical address */
CEC_PUBLIC unsigned char cec_addr_build(unsigned char source, unsigned char target)
{
	unsigned char ret = logical_address;
	asm("swap %[ret]" : [ret] "+r"(ret));
	return ret | target;
}

/* 0xff indicates we aren't ready */
CEC_PUBLIC bool cec_addr_ready(void)
{
	return logical_address != 0xff;
}

CEC_PUBLIC bool cec_addr_match(unsigned char addr)
{
	return addr == logical_address;
}

CEC_PUBLIC void cec_addr_init(void)
{
	logical_address = 0xff;
	cec_dev_idx = 0;
	transmit_state = TRANSMIT_IDLE;
}

/*
 * This will do work until we have a valid address or fallback to
 * the unregistered address.
 */
CEC_PUBLIC void cec_addr_periodic(void)
{
	if (cec_addr_ready())
		return;

	/* Throw away any messages */
	cec_receive_buf[CEC_RECEIVE_BUF_HDR] = 0;

	if (transmit_state == TRANSMIT_IDLE) {
		if (cec_dev_idx == sizeof(cec_dev_addrs))
			/* We failed, every address returned a reply */
			logical_address = CEC_ADDR_UNREGISTERED;

		else {
			/* Keep trying until we find a non-acked address */
			transmit_buf[0] = pgm_read_byte(cec_dev_addrs + cec_dev_idx);
			cec_dev_idx++;
			transmit_buf_end = 0;
			transmit_state = TRANSMIT_PEND;
		}

	} else if (transmit_state == TRANSMIT_FAILED)
		/* Found a non-acked address */
		logical_address = transmit_buf[0] & 0xf;
}
