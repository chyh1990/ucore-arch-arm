#ifndef __RIVER_EPROC_H__
#define __RIVER_EPROC_H__

#include <libs/types.h>
#include <proc/proc.h>
#include <proc/event.h>

typedef struct eproc_s *eproc_t;
typedef void (*eproc_idle_f) (void);

typedef struct eproc_s
{
	proc_s proc;
	event_pool_s event_pool;

	eproc_idle_f idle;
} eproc_s;

#define PROC_TO_EVENT_POOL(p) ((event_pool_t)(							\
		(uintptr_t)p +													\
		(uintptr_t)(&((eproc_t)0)->event_pool) - (uintptr_t)(&((eproc_t)0)->proc)))

#define EVENT_POOL_TO_PROC(ep) ((proc_t)(								\
		(uintptr_t)ep +													\
		(uintptr_t)(&((eproc_t)0)->proc) - (uintptr_t)(&((eproc_t)0)->event_pool)))

#define EVENT_POOL_TO_EPROC(ep) ((eproc_t)(								\
		(uintptr_t)ep - (uintptr_t)(&((eproc_t)0)->event_pool)))


int eproc_open(eproc_t eproc, const char *name, eproc_idle_f idle, void *private, size_t stack_size);

#endif
