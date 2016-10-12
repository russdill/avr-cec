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
#ifndef _CEC_H_
#define _CEC_H_

#include <avr/io.h>

#include <stdbool.h>

#define ARRAY_SIZE(n)	(sizeof(n) / sizeof((n)[0]))

#define CEC_BUFFER_SIZE 16

#ifndef CEC_PUBLIC
#define CEC_PUBLIC
#endif

#ifndef CEC_MONITOR
#define CEC_MONITOR 0
#endif

/* Error types */
#define CEC_ERR_NONE		0
#define CEC_ERR_ARB_LOST	1
#define CEC_ERR_NACK		2
#define CEC_ERR_NO_EOM		3
#define CEC_ERR_LOW_DRIVE	4
#define CEC_ERR_HALT		5
#define CEC_ERR_HW		6

#define CEC_STATUS_OVERRUN	_BV(6)
#define CEC_STATUS_NACK		_BV(7)

CEC_PUBLIC void cec_init(void);
CEC_PUBLIC void cec_halt(void) __attribute__((unused));
CEC_PUBLIC void cec_periodic(unsigned int delta);

CEC_PUBLIC void cec_transmit_init_hw(void);

CEC_PUBLIC bool cec_addr_match(unsigned char addr) __attribute__((unused));
CEC_PUBLIC void cec_addr_init(void);
CEC_PUBLIC void cec_addr_periodic(void);
CEC_PUBLIC bool cec_addr_ready(void) __attribute__((unused));
CEC_PUBLIC unsigned char cec_addr_build(unsigned char source,
			unsigned char target) __attribute__((unused));

/*
 * There is a pull-up resistor on the output line. If we set the output
 * as an input, the pull-up will drive the line high, which will drive
 * the CEC line low.
 */
#if CEC_MONITOR
#define cec_receive_drive_low() do {} while(0)
#define cec_receive_float() do {} while(0)
#define cec_receive_driven() (false)
#define cec_transmit_drive_low() do {} while(0)
#define cec_transmit_float() do {} while(0)
#define cec_pin_config() do {		\
	CEC_PORT |= _BV(CEC_PBIN);	\
} while (0)
#define cec_pin_unconfig() do { 	\
	CEC_PORT &= ~_BV(CEC_PBOUT);	\
	CEC_PORT &= ~_BV(CEC_PBIN);	\
} while (0)
#else

#define cec_receive_drive_low() do {	\
	CEC_DDR &= ~_BV(CEC_PBOUT);	\
} while(0)
#define cec_receive_float() do {	\
	CEC_DDR |= _BV(CEC_PBOUT);	\
} while (0)
#define cec_receive_driven()		(!(CEC_DDR & _BV(CEC_PBOUT)))
#define cec_transmit_drive_low() do {	\
	CEC_PORT |= _BV(CEC_PBOUT);	\
} while (0)
#define cec_transmit_float() do {	\
	CEC_PORT &= ~_BV(CEC_PBOUT); 	\
} while (0)

#define cec_output_state()		(!(CEC_PIN & _BV(CEC_PBOUT)))

#define cec_input_state()		(!(CEC_PIN & _BV(CEC_PBIN)))

#define cec_pin_config() do {		\
	CEC_DDR |= _BV(CEC_PBOUT);	\
	CEC_PORT |= _BV(CEC_PBIN);	\
	CEC_PORT |= _BV(CEC_PBOUT);	\
} while (0)
#define cec_pin_unconfig() do { 	\
	CEC_DDR &= ~_BV(CEC_PBOUT);	\
	CEC_PORT &= ~_BV(CEC_PBOUT);	\
	CEC_PORT &= ~_BV(CEC_PBIN);	\
} while (0)
#endif

#endif
