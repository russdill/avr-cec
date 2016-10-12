/*
 * Handle assigned logical addresses as a bitfield, this supports multiple
 * addresses simultaneously.
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
#include "cec.h"

CEC_PUBLIC unsigned short logical_addresses;

CEC_PUBLIC unsigned char cec_addr_build(unsigned char source, unsigned char target)
{
	return (source << 4) | target;
}

/* We don't bother checking if the address is really available */
CEC_PUBLIC bool cec_addr_ready(void)
{
	return true;
}

CEC_PUBLIC bool cec_addr_match(unsigned char addr)
{
	return (logical_addresses & (1 << addr)) != 0;
}

CEC_PUBLIC void cec_addr_init(void)
{
	logical_addresses = 0;
}

CEC_PUBLIC void cec_addr_periodic(void) {}
