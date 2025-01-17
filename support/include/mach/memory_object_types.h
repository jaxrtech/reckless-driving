/*
 * Copyright (c) 2000-2001 Apple Computer, Inc. All rights reserved.
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
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 */
/*
 *	File:	memory_object.h
 *	Author:	Michael Wayne Young
 *
 *	External memory management interface definition.
 */

#ifndef	_MACH_MEMORY_OBJECT_TYPES_H_
#define _MACH_MEMORY_OBJECT_TYPES_H_

/*
 *	User-visible types used in the external memory
 *	management interface:
 */

#include "port.h"
#include "vm_types.h"
#include "machine/vm_types.h"

#define VM_64_BIT_DATA_OBJECTS

typedef unsigned long long	memory_object_offset_t;
typedef unsigned long long	memory_object_size_t;

/*
 * Temporary until real EMMI version gets re-implemented
 */

typedef mach_port_t	memory_object_t;
typedef mach_port_t	memory_object_control_t;


typedef memory_object_t *memory_object_array_t;
					/* A memory object ... */
					/*  Used by the kernel to retrieve */
					/*  or store data */

typedef	mach_port_t	memory_object_name_t;
					/* Used to describe the memory ... */
					/*  object in vm_regions() calls */

typedef mach_port_t	memory_object_default_t;
					/* Registered with the host ... */
					/*  for creating new internal objects */

#define MEMORY_OBJECT_NULL		((memory_object_t) 0)
#define MEMORY_OBJECT_CONTROL_NULL	((memory_object_control_t) 0)
#define MEMORY_OBJECT_NAME_NULL		((memory_object_name_t) 0)
#define MEMORY_OBJECT_DEFAULT_NULL	((memory_object_default_t) 0)


typedef	int		memory_object_copy_strategy_t;
					/* How memory manager handles copy: */
#define		MEMORY_OBJECT_COPY_NONE		0
					/* ... No special support */
#define		MEMORY_OBJECT_COPY_CALL		1
					/* ... Make call on memory manager */
#define		MEMORY_OBJECT_COPY_DELAY 	2
					/* ... Memory manager doesn't
					 *     change data externally.
					 */
#define		MEMORY_OBJECT_COPY_TEMPORARY 	3
					/* ... Memory manager doesn't
					 *     change data externally, and
					 *     doesn't need to see changes.
					 */
#define		MEMORY_OBJECT_COPY_SYMMETRIC 	4
					/* ... Memory manager doesn't
					 *     change data externally,
					 *     doesn't need to see changes,
					 *     and object will not be
					 *     multiply mapped.
					 *
					 *     XXX
					 *     Not yet safe for non-kernel use.
					 */

#define		MEMORY_OBJECT_COPY_INVALID	5
					/* ...	An invalid copy strategy,
					 *	for external objects which
					 *	have not been initialized.
					 *	Allows copy_strategy to be
					 *	examined without also
					 *	examining pager_ready and
					 *	internal.
					 */

typedef	int		memory_object_return_t;
					/* Which pages to return to manager
					   this time (lock_request) */
#define		MEMORY_OBJECT_RETURN_NONE	0
					/* ... don't return any. */
#define		MEMORY_OBJECT_RETURN_DIRTY	1
					/* ... only dirty pages. */
#define		MEMORY_OBJECT_RETURN_ALL	2
					/* ... dirty and precious pages. */
#define		MEMORY_OBJECT_RETURN_ANYTHING	3
					/* ... any resident page. */

/* 
 *	Data lock request flags
 */

#define		MEMORY_OBJECT_DATA_FLUSH 	0x1
#define		MEMORY_OBJECT_DATA_NO_CHANGE	0x2
#define		MEMORY_OBJECT_DATA_PURGE	0x4
#define		MEMORY_OBJECT_COPY_SYNC		0x8
#define		MEMORY_OBJECT_DATA_SYNC		0x10

/*
 *	Types for the memory object flavor interfaces
 */

#define MEMORY_OBJECT_INFO_MAX      (1024) 
typedef int     *memory_object_info_t;      
typedef int	 memory_object_flavor_t;
typedef int      memory_object_info_data_t[MEMORY_OBJECT_INFO_MAX];


#define OLD_MEMORY_OBJECT_BEHAVIOR_INFO 	10	
#define MEMORY_OBJECT_PERFORMANCE_INFO	11
#define OLD_MEMORY_OBJECT_ATTRIBUTE_INFO	12
#define MEMORY_OBJECT_ATTRIBUTE_INFO	14
#define MEMORY_OBJECT_BEHAVIOR_INFO 	15	


struct old_memory_object_behave_info {
	memory_object_copy_strategy_t	copy_strategy;	
	boolean_t			temporary;
	boolean_t			invalidate;
};

struct memory_object_perf_info {
	vm_size_t			cluster_size;
	boolean_t			may_cache;
};

struct old_memory_object_attr_info {			/* old attr list */
        boolean_t       		object_ready;
        boolean_t       		may_cache;
        memory_object_copy_strategy_t 	copy_strategy;
};

struct memory_object_attr_info {
	memory_object_copy_strategy_t	copy_strategy;
	vm_offset_t			cluster_size;
	boolean_t			may_cache_object;
	boolean_t			temporary;
};

struct memory_object_behave_info {
	memory_object_copy_strategy_t	copy_strategy;	
	boolean_t			temporary;
	boolean_t			invalidate;
	boolean_t			silent_overwrite;
	boolean_t			advisory_pageout;
};

typedef struct old_memory_object_behave_info *old_memory_object_behave_info_t;
typedef struct old_memory_object_behave_info old_memory_object_behave_info_data_t;

typedef struct memory_object_behave_info *memory_object_behave_info_t;
typedef struct memory_object_behave_info memory_object_behave_info_data_t;

typedef struct memory_object_perf_info 	*memory_object_perf_info_t;
typedef struct memory_object_perf_info	memory_object_perf_info_data_t;

typedef struct old_memory_object_attr_info *old_memory_object_attr_info_t;
typedef struct old_memory_object_attr_info old_memory_object_attr_info_data_t;

typedef struct memory_object_attr_info	*memory_object_attr_info_t;
typedef struct memory_object_attr_info	memory_object_attr_info_data_t;

#define OLD_MEMORY_OBJECT_BEHAVE_INFO_COUNT   	\
                (sizeof(old_memory_object_behave_info_data_t)/sizeof(int))
#define MEMORY_OBJECT_BEHAVE_INFO_COUNT   	\
                (sizeof(memory_object_behave_info_data_t)/sizeof(int))
#define MEMORY_OBJECT_PERF_INFO_COUNT		\
		(sizeof(memory_object_perf_info_data_t)/sizeof(int))
#define OLD_MEMORY_OBJECT_ATTR_INFO_COUNT		\
		(sizeof(old_memory_object_attr_info_data_t)/sizeof(int))
#define MEMORY_OBJECT_ATTR_INFO_COUNT		\
		(sizeof(memory_object_attr_info_data_t)/sizeof(int))

#define invalid_memory_object_flavor(f)					\
	(f != MEMORY_OBJECT_ATTRIBUTE_INFO && 				\
	 f != MEMORY_OBJECT_PERFORMANCE_INFO && 			\
	 f != OLD_MEMORY_OBJECT_BEHAVIOR_INFO &&			\
	 f != MEMORY_OBJECT_BEHAVIOR_INFO &&				\
	 f != OLD_MEMORY_OBJECT_ATTRIBUTE_INFO)


/*
 * Used to support options on memory_object_release_name call
 */
#define MEMORY_OBJECT_TERMINATE_IDLE	0x1
#define MEMORY_OBJECT_RESPECT_CACHE	0x2
#define MEMORY_OBJECT_RELEASE_NO_OP	0x4


/*
 *  Universal Page List data structures
 */

#define MAX_UPL_TRANSFER 256

struct upl_page_info {
	vm_offset_t	phys_addr;
        unsigned int
                        pageout:1,      /* page is to be removed on commit */
                        absent:1,       /* No valid data in this page */
                        dirty:1,        /* Page must be cleaned (O) */
			precious:1,     /* must be cleaned, we have only copy */
			device:1,	/* no page data, mapped dev memory */
                        :0;		/* force to long boundary */
};

typedef struct upl_page_info	upl_page_info_t;
typedef upl_page_info_t		*upl_page_info_array_t;
typedef upl_page_info_array_t	upl_page_list_ptr_t;


/* upl invocation flags */

#define UPL_FLAGS_NONE		0x0
#define UPL_COPYOUT_FROM	0x1
#define UPL_PRECIOUS		0x2
#define UPL_NO_SYNC		0x4
#define UPL_CLEAN_IN_PLACE	0x8
#define UPL_NOBLOCK		0x10
#define UPL_RET_ONLY_DIRTY	0x20
#define UPL_SET_INTERNAL	0x40
#define	UPL_QUERY_OBJECT_TYPE	0x80

/* upl abort error flags */
#define UPL_ABORT_RESTART	0x1
#define UPL_ABORT_UNAVAILABLE	0x2
#define UPL_ABORT_ERROR		0x4
#define UPL_ABORT_FREE_ON_EMPTY	0x8  /* only implemented in wrappers */
#define UPL_ABORT_DUMP_PAGES	0x10
#define UPL_ABORT_NOTIFY_EMPTY	0x20

/* upl pages check flags */
#define UPL_CHECK_DIRTY         0x1

/* upl pagein/pageout  flags */
#define UPL_IOSYNC	0x1
#define UPL_NOCOMMIT	0x2
#define UPL_NORDAHEAD   0x4

/* upl commit flags */
#define UPL_COMMIT_FREE_ON_EMPTY	0x1 /* only implemented in wrappers */
#define UPL_COMMIT_CLEAR_DIRTY		0x2
#define UPL_COMMIT_SET_DIRTY		0x4
#define UPL_COMMIT_INACTIVATE		0x8
#define UPL_COMMIT_NOTIFY_EMPTY		0x10

/* flags for return of state from vm_map_get_upl,  vm_upl address space */
/* based call */
#define UPL_DEV_MEMORY			0x1
#define UPL_PHYS_CONTIG			0x2


/* access macros for upl_t */

#define UPL_DEVICE_PAGE(upl) \
	(((upl)[(index)].phys_addr != 0) ? (!((upl)[0].device)) : FALSE)

#define UPL_PAGE_PRESENT(upl, index)  \
	((upl)[(index)].phys_addr != 0)

#define UPL_PHYS_PAGE(upl, index) \
	(((upl)[(index)].phys_addr != 0) ?  \
			((upl)[(index)].phys_addr) : (vm_offset_t)NULL)

#define UPL_DIRTY_PAGE(upl, index) \
	(((upl)[(index)].phys_addr != 0) ? ((upl)[(index)].dirty) : FALSE)

#define UPL_PRECIOUS_PAGE(upl, index) \
	(((upl)[(index)].phys_addr != 0) ? ((upl)[(index)].precious) : FALSE)

#define UPL_VALID_PAGE(upl, index) \
	(((upl)[(index)].phys_addr != 0) ? (!((upl)[(index)].absent)) : FALSE)

#define UPL_PAGEOUT_PAGE(upl, index) \
	(((upl)[(index)].phys_addr != 0) ? ((upl)[(index)].pageout) : FALSE)

#define UPL_SET_PAGE_FREE_ON_COMMIT(upl, index) \
	if ((upl)[(index)].phys_addr != 0)     \
		((upl)[(index)].pageout) =  TRUE

#define UPL_CLR_PAGE_FREE_ON_COMMIT(upl, index) \
	if ((upl)[(index)].phys_addr != 0)     \
		((upl)[(index)].pageout) =  FALSE


/* 
 * Flags for the UPL page ops routine.  This routine is not exported
 * out of the kernel at the moment and so the defs live here.
 */
#define UPL_POP_DIRTY		0x1
#define UPL_POP_PAGEOUT		0x2
#define UPL_POP_PRECIOUS	0x4
#define UPL_POP_ABSENT		0x8
#define UPL_POP_BUSY		0x10

#define UPL_POP_PHYSICAL	0x10000000
#define UPL_POP_DUMP            0x20000000
#define UPL_POP_SET		0x40000000
#define UPL_POP_CLR		0x80000000



#endif	/* _MACH_MEMORY_OBJECT_TYPES_H_ */
