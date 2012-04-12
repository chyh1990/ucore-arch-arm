#include <glue_intr.h>
#include <proc/proc.h>
#include <mp/mp.h>
#include <mm/kmm.h>
#include <trap/trap.h>
#include <libs/string.h>
#include <proc/ipe.h>
#include <init.h>
#include <debug/io.h>

#define SCHED_NODE_TO_PROC(sch)											\
	((proc_t)((char *)(sch) - (uintptr_t)(&((proc_t)0)->sched_node)))

#define PROC_TIME_SLICE_DEFAULT 50

PLS volatile proc_t proc_current;
PLS static proc_s proc_idle;

static void
proc_switch(proc_t proc)
{
	proc_t old = proc_current;
	proc_current = proc;
	context_switch(&old->kern_ctx, &proc->kern_ctx);
}

void
proc_schedule(void)
{
	local_irq_save();
	int proc_runnable =
		proc_current->status == PROC_STATUS_RUNNABLE_WEAK ||
		proc_current->status == PROC_STATUS_RUNNABLE_STRONG;
	
	if (proc_current->time_slice == 0 || !proc_runnable)
	{
		if (proc_current->type != PROC_TYPE_IDLE && proc_runnable)
			sched_attach(&proc_current->sched_node);

		struct sched_node_s *s = sched_pick();
		proc_t nproc;
		  
		if (s != NULL)
		{
			sched_detach(s);
			nproc = SCHED_NODE_TO_PROC(s);
		}
		else
		{
			nproc = &proc_idle;
		}

		nproc->time_slice = PROC_TIME_SLICE_DEFAULT;
		if (nproc != proc_current)
		{
			proc_switch(nproc);
		}
	}
	local_irq_restore();
}

static void
proc_public_entry(void *__arg)
{
	proc_current->entry(__arg);
	kprintf("proc %s ends.\n", proc_current->name);
}

int
proc_open(proc_t proc, const char *name, proc_entry_f entry, void *arg, void *private, uintptr_t stack)
{
	if (name != NULL)
	{
		strncpy(proc->name, name, MAX_PROC_NAME);
	}
	else
	{
		strcpy(proc->name, "unnamed");
	}
	proc->name[MAX_PROC_NAME - 1] = 0;

	proc->type     = PROC_TYPE_KTHREAD;
	proc->private  = private;
	proc->entry    = entry;

	proc->irq_save_level = 0;
	 
	context_fill(&proc->kern_ctx, proc_public_entry, arg, stack);
	sched_node_init(&proc->sched_node);
	 
	proc->time_slice = PROC_TIME_SLICE_DEFAULT;
	proc->status     = PROC_STATUS_UNINIT;

	return 0;
}

void
do_idle(void)
{
	while (1)
	{
		if (init_finished)
			event_activate(ipe_event);
		proc_yield();
	}
}

int
proc_init(void)
{
	int err;
	if ((err = sched_init())) return err;

	/* XXX */
	event_pool_init(NULL, NULL, NULL, NULL);
	
	proc_t proc = &proc_idle;

	strcpy(proc->name, "idle");
	proc->name[MAX_PROC_NAME - 1] = 0;

	proc->type     = PROC_TYPE_IDLE;
	proc->entry    = NULL;
	proc->private  = NULL;

	proc->irq_save_level = 0;

	proc->kern_ctx.lcpu = lapic_id;
	proc->kern_ctx.stk_top = 0x0;
		
	sched_node_init(&proc->sched_node);

	proc->time_slice = PROC_TIME_SLICE_DEFAULT;
	proc->status     = PROC_STATUS_RUNNABLE_WEAK;

	proc_current = proc;
	return 0;
}

void
proc_yield(void)
{
	proc_current->time_slice = 0;
	proc_schedule();
}

void
proc_wait_pretend(void)
{
	int flag;
	local_intr_save_hw(flag);
	proc_current->status = PROC_STATUS_RUNNABLE_WEAK;
	local_intr_restore_hw(flag);
}

int
proc_wait_try(void)
{
	int flag, to_schedule = 0;
	local_intr_save_hw(flag);
	switch (proc_current->status)
	{
	case PROC_STATUS_RUNNABLE_WEAK:
		proc_current->status = PROC_STATUS_WAIT;
		to_schedule = 1;
		break;

	case PROC_STATUS_RUNNABLE_STRONG:
		proc_current->status = PROC_STATUS_RUNNABLE_WEAK;
		break;
	}
	local_intr_restore_hw(flag);

	if (to_schedule)
		proc_schedule();

	return to_schedule;
}

int
proc_notify(proc_t proc)
{
	int flag;
	local_intr_save_hw(flag);
	if (proc->status == PROC_STATUS_RUNNABLE_WEAK)
	{
		proc->status = PROC_STATUS_RUNNABLE_STRONG;
	}
	else if (proc->status == PROC_STATUS_WAIT)
	{
		proc->status = PROC_STATUS_RUNNABLE_WEAK;
		sched_attach(&proc->sched_node);
	}
	local_intr_restore_hw(flag);

	return 0;
}
