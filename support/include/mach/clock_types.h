/*
 * Copyright (c) 2000 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
 * @OSF_COPYRIGHT@
 */
/*
 *	File:		clock_types.h
 *	Purpose:	Clock facility header definitions. These
 *				definitons are needed by both kernel and
 *				user-level software.
 */

/*
 * N.B. This interface has been deprecated and the contents
 * of this file should be considered obsolete.
 */

#ifndef	_MACH_CLOCK_TYPES_H_
#define	_MACH_CLOCK_TYPES_H_

#include "time_value.h"

/*
 * Reserved clock id values for default clocks.
 */
#define SYSTEM_CLOCK		0	/* advances monotonically and
								 * uniformly; set to zero at boot */
#define CALENDAR_CLOCK		1	/* 'wall' clock; effectively
								 * synchronized to UTC */

#define REALTIME_CLOCK		0	/* obsolete; use SYSTEM or CALENDAR
								 * clock depending on particular
								 * requirements */

/*
 * Type definitions.
 */
typedef	int	alarm_type_t;		/* alarm time type */
typedef int	sleep_type_t;		/* sleep time type */
typedef	int	clock_id_t;			/* clock identification type */
typedef int	clock_flavor_t;		/* clock flavor type */
typedef int	*clock_attr_t;		/* clock attribute type */
typedef int	clock_res_t;		/* clock resolution type */

/*
 * Attribute names.
 */
#define	CLOCK_GET_TIME_RES	1	/* get_time call resolution */
/*							2	 * was map_time call resolution */
#define CLOCK_ALARM_CURRES	3	/* current alarm resolution */
#define CLOCK_ALARM_MINRES	4	/* minimum alarm resolution */
#define CLOCK_ALARM_MAXRES	5	/* maximum alarm resolution */

/*
 * Normal time specification used by the kernel clock facility.
 */
struct mach_timespec {
	unsigned int	tv_sec;			/* seconds */
	clock_res_t		tv_nsec;		/* nanoseconds */
};
typedef struct mach_timespec	mach_timespec_t;

#define NSEC_PER_USEC	1000		/* nanoseconds per microsecond */
#define USEC_PER_SEC	1000000		/* microseconds per second */
#define NSEC_PER_SEC	1000000000	/* nanoseconds per second */

#define BAD_MACH_TIMESPEC(t)						\
	((t)->tv_nsec < 0 || (t)->tv_nsec >= NSEC_PER_SEC)

/* t1 <=> t2, also (t1 - t2) in nsec with max of +- 1 sec */
#define CMP_MACH_TIMESPEC(t1, t2)					\
	((t1)->tv_sec > (t2)->tv_sec ? +NSEC_PER_SEC :	\
	((t1)->tv_sec < (t2)->tv_sec ? -NSEC_PER_SEC :	\
			(t1)->tv_nsec - (t2)->tv_nsec))

/* t1  += t2 */
#define ADD_MACH_TIMESPEC(t1, t2)								\
  do {															\
	if (((t1)->tv_nsec += (t2)->tv_nsec) >= NSEC_PER_SEC) {		\
		(t1)->tv_nsec -= NSEC_PER_SEC;							\
		(t1)->tv_sec  += 1;										\
	}															\
	(t1)->tv_sec += (t2)->tv_sec;								\
  } while (0)

/* t1  -= t2 */
#define SUB_MACH_TIMESPEC(t1, t2)								\
  do {															\
	if (((t1)->tv_nsec -= (t2)->tv_nsec) < 0) {					\
		(t1)->tv_nsec += NSEC_PER_SEC;							\
		(t1)->tv_sec  -= 1;										\
	}															\
	(t1)->tv_sec -= (t2)->tv_sec;								\
  } while (0)

/*
 * Alarm parameter defines.
 */
#define ALRMTYPE			0xff		/* type (8-bit field)	*/
#define TIME_ABSOLUTE		0x00		/* absolute time */
#define	TIME_RELATIVE		0x01		/* relative time */

#define BAD_ALRMTYPE(t)		(((t) &~ TIME_RELATIVE) != 0)

#endif /* _MACH_CLOCK_TYPES_H_ */
