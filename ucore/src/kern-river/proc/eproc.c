#include <proc/eproc.h>
#include <glue_mmu.h>
#include <glue_memlayout.h>
#include <glue_pmm.h>

static void
eproc_public_entry(void *__arg)
{
	event_loop(PROC_TO_EVENT_POOL(proc_current));
}

static int
ep_touch(event_t event)
{
	proc_notify(EVENT_POOL_TO_PROC(event->pool));
	return 0;
}

static void
ep_exhaust(event_pool_t pool)
{
	EVENT_POOL_TO_EPROC(pool)->idle();
}

static void
ep_stop(event_pool_t pool)
{
	// proc_t proc = EVENT_POOL_TO_PROC(pool);
	/* XXX */
}

int
eproc_open(eproc_t eproc, const char *name, eproc_idle_f idle, void *private, size_t stack_size)
{
	int stack_pages = (stack_size + PGSIZE - 1) >> PGSHIFT;

	eproc->idle = idle;
	event_pool_init(&eproc->event_pool, ep_touch, ep_exhaust, ep_stop);
	proc_open(&eproc->proc, name, eproc_public_entry, NULL,  private,
			  (uintptr_t)KADDR_DIRECT(kalloc_pages(stack_pages)) + (stack_pages << PGSHIFT));

	eproc->proc.type   = PROC_TYPE_KEVENTD;
	eproc->proc.status = PROC_STATUS_WAIT;

	return 0;
}
