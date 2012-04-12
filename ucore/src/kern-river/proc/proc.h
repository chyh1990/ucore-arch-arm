#ifndef __RIVER_PROC_H__
#define __RIVER_PROC_H__

#include <libs/types.h>
#include <glue_context.h>
#include <glue_mp.h>

#include <proc/sched.h>
#include <proc/event.h>

#define MAX_PROC_NAME 32

typedef struct proc_s *proc_t;
typedef void(*proc_entry_f)(void *arg);

typedef struct proc_s
{
	uint32_t     type;
	uint32_t     status;
	char         name[MAX_PROC_NAME];
	
	context_s    kern_ctx;
	sched_node_s sched_node;
	uint32_t     time_slice;
	uint32_t     irq_save_level;

	proc_entry_f entry;
	void        *private;
} proc_s;

#define PROC_TYPE_KTHREAD 0x0
#define PROC_TYPE_KEVENTD 0x1
#define PROC_TYPE_UTHREAD 0x2
#define PROC_TYPE_IDLE    0x3

#define PROC_STATUS_UNINIT          0
#define PROC_STATUS_RUNNABLE_WEAK   1
#define PROC_STATUS_RUNNABLE_STRONG 2
#define PROC_STATUS_WAIT            3
#define PROC_STATUS_ZOMBIE          4

extern volatile proc_t proc_current;

int  proc_init(void);
int  proc_open(proc_t proc, const char *name, proc_entry_f entry, void *arg, void *private, uintptr_t stack);
void proc_schedule(void);
void proc_yield(void);
void proc_wait_pretend(void);
int  proc_wait_try(void);
int  proc_notify(proc_t proc);
int  proc_close(proc_t proc);
int  proc_exit(void);

void do_idle(void);

#endif
