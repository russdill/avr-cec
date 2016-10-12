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
#include "cec.h"

#if CEC_MONITOR
static void cec_transmit_receive_ack(bool bit) {}
static void cec_transmit_on_error(void) {}
static void cec_check_tx_bit(bool bit) {}
static void cec_transmit_periodic(unsigned int delta) {}
static void cec_transmit_halt(void) {}
static inline void cec_transmit_init(void) {}
#else
#include "cec_transmit.c"
#endif

#include "cec_receive.c"

#ifdef CEC_USI
#include "cec_usi.c"
#else
#include "cec_receive_min.c"
#if CEC_MONITOR
/* No transmit capability */
#elif defined(CEC_TRANSMIT_PWM)
#include "cec_transmit_pwm.c"
#else
#include "cec_transmit_raw.c"
#endif
#endif

#ifdef CEC_DEV_TYPE
#include "cec_addr_dev_type.c"
#elif defined(CEC_LOGICAL_ADDRESS_BITFIELD)
#include "cec_addr_bitfield.c"
#elif defined(CEC_FIXED_LOGICAL_ADDRESS)
#include "cec_addr_fixed.c"
#else
#include "cec_addr_none.c"
#endif

CEC_PUBLIC void cec_init(void)
{
	cec_pin_config();
	cec_receive_init();
	cec_transmit_init();
	cec_addr_init();
}

CEC_PUBLIC void cec_periodic(unsigned int delta)
{
	cec_receive_periodic(delta);
	cec_transmit_periodic(delta);
	cec_addr_periodic();
}

CEC_PUBLIC void cec_halt(void)
{
	cec_receive_error(CEC_ERR_HALT);
	cec_receive_halt();
	cec_transmit_halt();
	cec_addr_init();
}
