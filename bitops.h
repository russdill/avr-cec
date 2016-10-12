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
#ifndef _BITOPS_H_
#define _BITOPS_H_

#ifdef __ASSEMBLER__

/* We can't use ternary in asm, use this sillyness instead */
#define __LOG2_ENTRY(n, bit) ( (- ((n) >= _BV(bit) && (n) < _BV(bit+1)) ) & bit)

#define LOG2(n) ( \
	__LOG2_ENTRY(n, 0) | \
	__LOG2_ENTRY(n, 1) | \
	__LOG2_ENTRY(n, 2) | \
	__LOG2_ENTRY(n, 3) | \
	__LOG2_ENTRY(n, 4) | \
	__LOG2_ENTRY(n, 5) | \
	__LOG2_ENTRY(n, 6) | \
	__LOG2_ENTRY(n, 7) | \
	__LOG2_ENTRY(n, 8) | \
	__LOG2_ENTRY(n, 9) | \
	__LOG2_ENTRY(n, 10) | \
	__LOG2_ENTRY(n, 11) | \
	__LOG2_ENTRY(n, 12) | \
	__LOG2_ENTRY(n, 13) | \
	__LOG2_ENTRY(n, 14) | \
	__LOG2_ENTRY(n, 15) | \
	__LOG2_ENTRY(n, 16) | \
	__LOG2_ENTRY(n, 17) | \
	__LOG2_ENTRY(n, 18) | \
	__LOG2_ENTRY(n, 19) | \
	__LOG2_ENTRY(n, 20) | \
	__LOG2_ENTRY(n, 21) | \
	__LOG2_ENTRY(n, 22) | \
	__LOG2_ENTRY(n, 23) | \
	__LOG2_ENTRY(n, 24) | \
	__LOG2_ENTRY(n, 25) | \
	__LOG2_ENTRY(n, 26) | \
	__LOG2_ENTRY(n, 27) | \
	__LOG2_ENTRY(n, 28) | \
	__LOG2_ENTRY(n, 29) | \
	__LOG2_ENTRY(n, 30) | \
	((-((n) > _BV(31))) & 31))

#else

#define LOG2(n) ( \
	(n) < _BV(0) ? 0 : \
	(n) < _BV(1) ? 0 : \
	(n) < _BV(2) ? 1 : \
	(n) < _BV(3) ? 2 : \
	(n) < _BV(4) ? 3 : \
	(n) < _BV(5) ? 4 : \
	(n) < _BV(6) ? 5 : \
	(n) < _BV(7) ? 6 : \
	(n) < _BV(8) ? 7 : \
	(n) < _BV(9) ? 8 : \
	(n) < _BV(10) ? 9 : \
	(n) < _BV(11) ? 10 : \
	(n) < _BV(12) ? 11 : \
	(n) < _BV(13) ? 12 : \
	(n) < _BV(14) ? 13 : \
	(n) < _BV(15) ? 14 : \
	(n) < _BV(16) ? 15 : \
	(n) < _BV(17) ? 16 : \
	(n) < _BV(18) ? 17 : \
	(n) < _BV(19) ? 18 : \
	(n) < _BV(20) ? 19 : \
	(n) < _BV(21) ? 20 : \
	(n) < _BV(22) ? 21 : \
	(n) < _BV(23) ? 22 : \
	(n) < _BV(24) ? 23 : \
	(n) < _BV(25) ? 24 : \
	(n) < _BV(26) ? 25 : \
	(n) < _BV(27) ? 26 : \
	(n) < _BV(28) ? 27 : \
	(n) < _BV(29) ? 28 : \
	(n) < _BV(30) ? 29 : \
	(n) < _BV(31) ? 30 : 31)
#endif

#endif
