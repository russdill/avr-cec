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
#ifndef _CEC_SPEC_H_
#define _CEC_SPEC_H_

#ifndef __ASSEMBLER__
enum {
	CEC_ADDR_TV,
	CEC_ADDR_RECORDING_DEVICE_1,
	CEC_ADDR_RECORDING_DEVICE_2,
	CEC_ADDR_TUNER_1,
	CEC_ADDR_PLAYBACK_DEVICE_1,
	CEC_ADDR_AUDIO_SYSTEM,
	CEC_ADDR_TUNER_2,
	CEC_ADDR_TUNER_3,
	CEC_ADDR_PLAYBACK_DEVICE_2,
	CEC_ADDR_RECORDING_DEVICE_3,
	CEC_ADDR_TUNER_4,
	CEC_ADDR_PLAYBACK_DEVICE_3,
	CEC_ADDR_RESERVED_1,
	CEC_ADDR_RESERVED_2,
	CEC_ADDR_SPECIFIC_USE,
	CEC_ADDR_UNREGISTERED,
	CEC_ADDR_BROADCAST = 0xf,
};

#endif

#define CEC_DEV_TV			0
#define CEC_DEV_RECORDING_DEVICE	1
#define CEC_DEV_TUNER			3
#define CEC_DEV_PLAYBACK_DEVICE		4
#define CEC_DEV_AUDIO_SYSTEM		5
#define CEC_DEV_SWITCH			6
#define CEC_DEV_VIDEO_PROCESSOR		7

#define _A(n)				((n) | ((n) << 4))

#define CEC_DEV_TV_ADDRS		_A(CEC_ADDR_TV), \
					_A(CEC_ADDR_SPECIFIC_USE)
#define CEC_DEV_RECORDING_DEVICE_ADDRS	_A(CEC_ADDR_RECORDING_DEVICE_1), \
					_A(CEC_ADDR_RECORDING_DEVICE_2), \
					_A(CEC_ADDR_RECORDING_DEVICE_3)
#define CEC_DEV_TUNER_ADDRS		_A(CEC_ADDR_TUNER_1), \
					_A(CEC_ADDR_TUNER_2), \
					_A(CEC_ADDR_TUNER_3), \
					_A(CEC_ADDR_TUNER_4)
#define CEC_DEV_PLAYBACK_DEVICE_ADDRS	_A(CEC_ADDR_PLAYBACK_DEVICE_1), \
					_A(CEC_ADDR_PLAYBACK_DEVICE_2), \
					_A(CEC_ADDR_PLAYBACK_DEVICE_3)
#define CEC_DEV_AUDIO_SYSTEM_ADDRS	_A(CEC_ADDR_AUDIO_SYSTEM)
#define CEC_DEV_SWITCH_ADDRS
#define CEC_DEV_VIDEO_PROCESSOR_ADDRS	_A(CEC_ADDR_SPECIFIC_USE)

#define CEC_T0			0
#define CEC_T1_EARLY1		400
#define CEC_1			600
#define CEC_T2_LATE1		800
#define CEC_T3			850
#define CEC_NOM_SAMPLE		1050
#define CEC_T4			1250
#define CEC_T5_EARLY0		1300
#define CEC_0			1500
#define CEC_T6_LATE0		1700
#define CEC_T7_EARLY_END	2050
#define CEC_PERIOD		2400
#define CEC_T8_LATE_END		2750

#define CEC_START_LOW_EARLY	3500
#define CEC_START_LOW		3700
#define CEC_START_LOW_LATE	3900

#define CEC_START_HIGH_EARLY	4300
#define CEC_START_HIGH		4500
#define CEC_START_HIGH_LATE	4700

#define CEC_PRESENT_PERIOD_WAIT	7
#define CEC_NEW_PERIOD_WAIT	5
#define CEC_PREV_PERIOD_WAIT	3

#define CEC_MAX_RISE_TIME	250
#define CEC_MAX_FALL_TIME	50

/*
 * Spec is a bit ambiguous, 'Present initiator wants to send another frame
 * immediately after its previous frame'. Just choose 1 second.
 */
#define PRES_INIT_TIME		1000000

#define MAKE_ADDRESS(s, d) (((s) << 4) | (d))

#define CEC_XMIT_MAX_RETRANSMIT	5

#endif
