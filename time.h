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
#ifndef _TIME_H_
#define _TIME_H_

#ifndef TIME_PUBLIC
#define TIME_PUBLIC
#endif

#if defined(__ASSEMBLER__) && defined(_F_CPU)
#undef F_CPU
#define F_CPU _F_CPU
#endif

#include "div.h"

/* Ideally, we count to 256 */
#ifdef TCNT0_ROLLOVER_PERIOD_US
#define TCNT0_PRESCALER_IDEAL	DIV_ROUND_UP(F_CPU * \
				TCNT0_ROLLOVER_PERIOD_US, 256000000)
#endif

#ifdef TCNT0_ROLLOVER_HZ
#define TCNT0_PRESCALER_IDEAL	DIV_ROUND_UP(F_CPU, 256 * TCNT0_ROLLOVER_HZ)
#endif

#ifndef TCNT0_PRESCALER_IDEAL
#error "No prescaler defined"
#endif

/* But if we can't, we must count to a lower value */
#ifdef __ASSEMBLER__
#if TCNT0_PRESCALER_IDEAL == 1
#define TCNT0_PRESCALER 1
#define TCNT0_PRESCALER_VAL 1
#elif TCNT0_PRESCALER_IDEAL <= 8
#define TCNT0_PRESCALER 8
#define TCNT0_PRESCALER_VAL 2
#elif TCNT0_PRESCALER_IDEAL <= 64
#define TCNT0_PRESCALER 64
#define TCNT0_PRESCALER_VAL 3
#elif TCNT0_PRESCALER_IDEAL <= 256
#define TCNT0_PRESCALER 256
#define TCNT0_PRESCALER_VAL 4
#elif TCNT0_PRESCALER_IDEAL <= 1024
#define TCNT0_PRESCALER 1024
#define TCNT0_PRESCALER_VAL 5
#else
#error "Could not find valid prescaler"
#endif

#else

#if TCNT0_PRESCALER_IDEAL == 1
#define TCNT0_PRESCALER 1UL
#define TCNT0_PRESCALER_VAL 1
#elif TCNT0_PRESCALER_IDEAL <= 8
#define TCNT0_PRESCALER 8UL
#define TCNT0_PRESCALER_VAL 2
#elif TCNT0_PRESCALER_IDEAL <= 64
#define TCNT0_PRESCALER 64UL
#define TCNT0_PRESCALER_VAL 3
#elif TCNT0_PRESCALER_IDEAL <= 256
#define TCNT0_PRESCALER 256UL
#define TCNT0_PRESCALER_VAL 4
#elif TCNT0_PRESCALER_IDEAL <= 1024
#define TCNT0_PRESCALER 1024UL
#define TCNT0_PRESCALER_VAL 5
#else
#error "Could not find valid prescaler"
#endif

#endif

#define S_TO_JIFFIES_RND(n)	DIV_ROUND((n) * F_CPU, TCNT0_PRESCALER)
#define MS_TO_JIFFIES_RND(n)	DIV_ROUND((n) * F_CPU, TCNT0_PRESCALER * 1000)
#define US_TO_JIFFIES_RND(n)	DIV_ROUND((n) * F_CPU, TCNT0_PRESCALER * 1000000)
#define NS_TO_JIFFIES_RND(n)	DIV_ROUND((n) * F_CPU, TCNT0_PRESCALER * 1000000000)

#define S_TO_JIFFIES_UP(n)	DIV_ROUND_UP((n) * F_CPU, TCNT0_PRESCALER)
#define MS_TO_JIFFIES_UP(n)	DIV_ROUND_UP((n) * F_CPU, TCNT0_PRESCALER * 1000)
#define US_TO_JIFFIES_UP(n)	DIV_ROUND_UP((n) * F_CPU, TCNT0_PRESCALER * 1000000)
#define NS_TO_JIFFIES_UP(n)	DIV_ROUND_UP((n) * F_CPU, TCNT0_PRESCALER * 1000000000)

#define S_TO_JIFFIES(n)		(((n) * F_CPU) / TCNT0_PRESCALER)
#define MS_TO_JIFFIES(n)	(((n) * F_CPU) / (TCNT0_PRESCALER * 1000))
#define US_TO_JIFFIES(n)	(((n) * F_CPU) / (TCNT0_PRESCALER * 1000000))
#define NS_TO_JIFFIES(n)	(((n) * F_CPU) / (TCNT0_PRESCALER * 1000000000))

#define HZ_TO_JIFFIES_RND(n)	DIV_ROUND(F_CPU, TCNT0_PRESCALER * (n))
#define KHZ_TO_JIFFIES_RND(n)	DIV_ROUND(1000 * F_CPU, TCNT0_PRESCALER * (n))
#define MHZ_TO_JIFFIES_RND(n)	DIV_ROUND(1000000 * F_CPU, TCNT0_PRESCALER * (n))
#define GHZ_TO_JIFFIES_RND(n)	DIV_ROUND(1000000000 * F_CPU, TCNT0_PRESCALER * (n))

#define HZ_TO_JIFFIES_UP(n)	DIV_ROUND_UP(F_CPU, TCNT0_PRESCALER * (n))
#define KHZ_TO_JIFFIES_UP(n)	DIV_ROUND_UP(1000 * F_CPU, TCNT0_PRESCALER * (n))
#define MHZ_TO_JIFFIES_UP(n)	DIV_ROUND_UP(1000000 * F_CPU, TCNT0_PRESCALER * (n))
#define GHZ_TO_JIFFIES_UP(n)	DIV_ROUND_UP(1000000000 * F_CPU, TCNT0_PRESCALER * (n))

#define HZ_TO_JIFFIES(n)	(F_CPU / (TCNT0_PRESCALER * (n)))
#define KHZ_TO_JIFFIES(n)	((1000) * F_CPU / (TCNT0_PRESCALER * (n)))
#define MHZ_TO_JIFFIES(n)	((1000000) * F_CPU / (TCNT0_PRESCALER * (n)))
#define GHZ_TO_JIFFIES(n)	((1000000000) * F_CPU) / (TCNT0_PRESCALER * (n)))


/* For 8 bit types that track longer time periods */
#ifdef LONG_TIME_S
#define LJIFFIES_MAX S_TO_JIFFIES_UP(LONG_TIME_S)
#elif defined(LONG_TIME_MS)
#define LJIFFIES_MAX MS_TO_JIFFIES_UP(LONG_TIME_MS)
#elif defined(LONG_TIME_US)
#define LJIFFIES_MAX US_TO_JIFFIES_UP(LONG_TIME_US)
#elif defined(LONG_TIME_NS)
#define LJIFFIES_MAX NS_TO_JIFFIES_UP(LONG_TIME_NS)
#endif

#ifdef LJIFFIES_MAX
#if (LJIFFIES_MAX) < (1UL << 8)
#define LJIFFIES_SHIFT 0
#elif (LJIFFIES_MAX) < (1UL << 9)
#define LJIFFIES_SHIFT 1
#elif (LJIFFIES_MAX) < (1UL << 10)
#define LJIFFIES_SHIFT 2
#elif (LJIFFIES_MAX) < (1UL << 11)
#define LJIFFIES_SHIFT 3
#elif (LJIFFIES_MAX) < (1UL << 12)
#define LJIFFIES_SHIFT 4
#elif (LJIFFIES_MAX) < (1UL << 13)
#define LJIFFIES_SHIFT 5
#elif (LJIFFIES_MAX) < (1UL << 14)
#define LJIFFIES_SHIFT 6
#elif (LJIFFIES_MAX) < (1UL << 15)
#define LJIFFIES_SHIFT 7
#elif (LJIFFIES_MAX) < (1UL << 16)
#define LJIFFIES_SHIFT 8
#elif (LJIFFIES_MAX) < (1UL << 17)
#define LJIFFIES_SHIFT 9
#elif (LJIFFIES_MAX) < (1UL << 18)
#define LJIFFIES_SHIFT 10
#elif (LJIFFIES_MAX) < (1UL << 19)
#define LJIFFIES_SHIFT 11
#elif (LJIFFIES_MAX) < (1UL << 20)
#define LJIFFIES_SHIFT 12
#elif (LJIFFIES_MAX) < (1UL << 21)
#define LJIFFIES_SHIFT 13
#elif (LJIFFIES_MAX) < (1UL << 22)
#define LJIFFIES_SHIFT 14
#elif (LJIFFIES_MAX) < (1UL << 23)
#define LJIFFIES_SHIFT 15
#elif (LJIFFIES_MAX) < (1UL << 24)
#define LJIFFIES_SHIFT 16
#elif (LJIFFIES_MAX) < (1UL << 25)
#define LJIFFIES_SHIFT 17
#elif (LJIFFIES_MAX) < (1UL << 26)
#define LJIFFIES_SHIFT 18
#elif (LJIFFIES_MAX) < (1UL << 27)
#define LJIFFIES_SHIFT 19
#elif (LJIFFIES_MAX) < (1UL << 28)
#define LJIFFIES_SHIFT 20
#elif (LJIFFIES_MAX) < (1UL << 29)
#define LJIFFIES_SHIFT 21
#elif (LJIFFIES_MAX) < (1UL << 30)
#define LJIFFIES_SHIFT 22
#elif (LJIFFIES_MAX) < (1UL << 31)
#define LJIFFIES_SHIFT 23
#else
#define LJIFFIES_SHIFT 24
#endif

#define TCNT0_LPRESCALER	((1 << LJIFFIES_SHIFT) * TCNT0_PRESCALER)

#define S_TO_LJIFFIES_RND(n)	DIV_ROUND((n) * F_CPU, TCNT0_LPRESCALER)
#define MS_TO_LJIFFIES_RND(n)	DIV_ROUND((n) * F_CPU, TCNT0_LPRESCALER * 1000)
#define US_TO_LJIFFIES_RND(n)	DIV_ROUND((n) * F_CPU, TCNT0_LPRESCALER * 1000000)
#define NS_TO_LJIFFIES_RND(n)	DIV_ROUND((n) * F_CPU, TCNT0_LPRESCALER * 1000000000)

#define S_TO_LJIFFIES_UP(n)	DIV_ROUND_UP((n) * F_CPU, TCNT0_LPRESCALER)
#define MS_TO_LJIFFIES_UP(n)	DIV_ROUND_UP((n) * F_CPU, TCNT0_LPRESCALER * 1000)
#define US_TO_LJIFFIES_UP(n)	DIV_ROUND_UP((n) * F_CPU, TCNT0_LPRESCALER * 1000000)
#define NS_TO_LJIFFIES_UP(n)	DIV_ROUND_UP((n) * F_CPU, TCNT0_LPRESCALER * 1000000000)

#define S_TO_LJIFFIES(n)	(((n) * F_CPU) / TCNT0_LPRESCALER)
#define MS_TO_LJIFFIES(n)	(((n) * F_CPU) / (TCNT0_LPRESCALER * 1000))
#define US_TO_LJIFFIES(n)	(((n) * F_CPU) / (TCNT0_LPRESCALER * 1000000))
#define NS_TO_LJIFFIES(n)	(((n) * F_CPU) / (TCNT0_LPRESCALER * 1000000000))

#define HZ_TO_LJIFFIES_RND(n)	DIV_ROUND(F_CPU, TCNT0_LPRESCALER * (n))
#define KHZ_TO_LJIFFIES_RND(n)	DIV_ROUND(1000 * F_CPU, TCNT0_LPRESCALER * (n))
#define MHZ_TO_LJIFFIES_RND(n)	DIV_ROUND(1000000 * F_CPU, TCNT0_LPRESCALER * (n))
#define GHZ_TO_LJIFFIES_RND(n)	DIV_ROUND(1000000000 * F_CPU, TCNT0_LPRESCALER * (n))

#define HZ_TO_LJIFFIES_UP(n)	DIV_ROUND_UP(F_CPU, TCNT0_LPRESCALER * (n))
#define KHZ_TO_LJIFFIES_UP(n)	DIV_ROUND_UP(1000 * F_CPU, TCNT0_LPRESCALER * (n))
#define MHZ_TO_LJIFFIES_UP(n)	DIV_ROUND_UP(1000000 * F_CPU, TCNT0_LPRESCALER * (n))
#define GHZ_TO_LJIFFIES_UP(n)	DIV_ROUND_UP(1000000000 * F_CPU, TCNT0_LPRESCALER * (n))

#define HZ_TO_LJIFFIES(n)	(F_CPU / (TCNT0_LPRESCALER * (n)))
#define KHZ_TO_LJIFFIES(n)	((1000 * F_CPU) / (TCNT0_LPRESCALER * (n)))
#define MHZ_TO_LJIFFIES(n)	((1000000 * F_CPU) / (TCNT0_LPRESCALER * (n)))
#define GHZ_TO_LJIFFIES(n)	((1000000000 * F_CPU) / (TCNT0_LPRESCALER * (n)))
#endif

#ifndef __ASSEMBLER__
__uint24 jiffies(void);
#endif

#endif
