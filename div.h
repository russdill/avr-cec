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
#ifndef _DIV_H_
#define _DIV_H_

#ifndef __ASSEMBLER__
#define DIV_ROUND(n, d)		(((n) + (d) / 2UL) / (d))
#define DIV_ROUND_UP(n, d)	(1 + ((n) - 1UL) / (d))
#else
#define DIV_ROUND(n, d)		(((n) + (d) / 2) / (d))
#define DIV_ROUND_UP(n, d)	(1 + ((n) - 1) / (d))
#endif

#define _MUL_BIT_FIXED_B(a, bn, bd, max_a, bit, ret)	do {	\
	if ((max_a) >= (1 << (bit)) && ((a) & (1 << (bit))))	\
		ret += (1 << (bit)) * (bn) / (bd);		\
} while(0)

#define _MUL_FIXED_B(a, bn, bd, max_a, type) ({		\
	type z = 0;					\
	_MUL_BIT_FIXED_B(a, bn, bd, max_a, 0, z);	\
	_MUL_BIT_FIXED_B(a, bn, bd, max_a, 1, z);	\
	_MUL_BIT_FIXED_B(a, bn, bd, max_a, 2, z);	\
	_MUL_BIT_FIXED_B(a, bn, bd, max_a, 3, z);	\
	_MUL_BIT_FIXED_B(a, bn, bd, max_a, 4, z);	\
	_MUL_BIT_FIXED_B(a, bn, bd, max_a, 5, z);	\
	_MUL_BIT_FIXED_B(a, bn, bd, max_a, 6, z);	\
	_MUL_BIT_FIXED_B(a, bn, bd, max_a, 7, z);	\
	z;						\
})

#define _MUL_SHIFT_FIXED(a, bn, bd, bit)				\
	if (bd == (bn) * (1 << (bit))) ret = (a) << (bit);		\
	else if (bd == (bn) / (1 << (bit))) ret = (a) >> (bit)

#define MUL_FIXED_B(a, bn, bd, max_a) ({				\
	unsigned long ret;						\
	if (bd == bn) ret = a;						\
	else _MUL_SHIFT_FIXED(a, bn, bd, 1);				\
	else _MUL_SHIFT_FIXED(a, bn, bd, 2);				\
	else _MUL_SHIFT_FIXED(a, bn, bd, 3);				\
	else _MUL_SHIFT_FIXED(a, bn, bd, 4);				\
	else _MUL_SHIFT_FIXED(a, bn, bd, 5);				\
	else _MUL_SHIFT_FIXED(a, bn, bd, 6);				\
	else _MUL_SHIFT_FIXED(a, bn, bd, 7);				\
	else if ((max_a) * (bn) / (bd) < 256)				\
		ret = _MUL_FIXED_B(a, bn, bd, max_a, unsigned char);	\
	else if ((max_a) * (bn) / (bd) < 65536)				\
		ret = _MUL_FIXED_B(a, bn, bd, max_a, unsigned int);	\
	else								\
		ret = _MUL_FIXED_B(a, bn, bd, max_a, unsigned long);	\
	ret;								\
})

#endif
