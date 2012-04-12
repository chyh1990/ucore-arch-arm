#include <proc/ipe.h>
#include <glue_memlayout.h>
#include <libs/types.h>
#include <glue_mp.h>
#include <mp/mp.h>
#include <debug/io.h>
#include <glue_pmm.h>
#include <libs/x86/atom.h>
#include <drivers/rand.h>
#include <drivers/timer.h>
#include <proc/eproc.h>

#define RBUF_PAGES 16
#define RBUF_SIZE  (RBUF_PAGES << PGSHIFT)
#define RBUF_COUNT (RBUF_SIZE / sizeof(ipe_packet_t))

#define IPE_RETRY 10
#define IPE_REFRESH_INV 1

PLS static volatile ipe_packet_t *local_recv_buffer;
static volatile ipe_packet_t *remote_recv_buffer[LAPIC_COUNT];

PLS static eproc_s ipe_eproc;
PLS static event_s __ipe_event;
PLS event_t ipe_event = &__ipe_event;
PLS static timer_s ipe_timer;

static void
ipe_packet_handle(ipe_packet_t packet)
{
	packet->handler(packet);
}

static void
ipe_packet_back_handle(ipe_packet_t packet)
{
	packet->back_handler(packet);
}

static void
do_ipe(event_t e)
{
	int i;
	for (i = 0; i != RBUF_COUNT; ++ i)
	{
		if (local_recv_buffer[i])
		{
			ipe_packet_t packet = local_recv_buffer[i];
			if (packet->from_lapic == lapic_id)
			{
				ipe_packet_back_handle(packet);
			}
			else
			{
				if (packet->processed == 0)
				{
					packet->processed = 1;
					ipe_packet_handle(packet);
				}

				if (ipe_packet_send(packet->from_lapic, packet))
				{
					/* SEND ACK FAILED */
					continue;
				}
			}
			
			local_recv_buffer[i] = NULL;
		}
	}

	if (e == &ipe_timer.event)
	{
		timer_open(&ipe_timer, timer_tick + IPE_REFRESH_INV * timer_freq);
	}
}

#include <libs/x86/spinlock.h>

volatile static int ipe_ready = 0;
spinlock_s ipe_init_alloc_lock = {0};

int
ipe_init(void)
{
	spinlock_acquire(&ipe_init_alloc_lock);
	local_recv_buffer = remote_recv_buffer[lapic_id] =
		(ipe_packet_t *)KADDR_DIRECT(kalloc_pages(RBUF_PAGES));
	spinlock_release(&ipe_init_alloc_lock);
	
	eproc_open(&ipe_eproc, "ipe", (void(*)(void))proc_wait_try, NULL, 8192);
	event_open(ipe_event, &ipe_eproc.event_pool, do_ipe, NULL);
	event_open(&ipe_timer.event, &ipe_eproc.event_pool, do_ipe, NULL);
	timer_open(&ipe_timer, timer_tick + IPE_REFRESH_INV * timer_freq);

	/* ALL CPU BARRIER {{{ */	
	/* XXX: use naive CAS here =_= for atomic inc */
	while (1)
	{
		int old = ipe_ready;
		if (cmpxchg32(&ipe_ready, old, old + 1) == old) break;
	}
	
	while (ipe_ready != lcpu_count) ;
	/* }}} */
	
	return 0;
}

void
ipe_packet_init(ipe_packet_t packet, ipe_packet_handler_f handler, ipe_packet_handler_f back_handler, void *private)
{
	packet->from_lapic   = lapic_id;
	packet->handler      = handler;
	packet->back_handler = back_handler;
	packet->private      = private;
}

int
ipe_packet_send(int target_lapic_id, ipe_packet_t packet)
{
	if (lapic_id == target_lapic_id)
	{
		ipe_packet_handle(packet);
		ipe_packet_back_handle(packet);
		return 0;
	}

	packet->processed = 0;
	
	int retry;
	for (retry = 0; retry < IPE_RETRY; ++ retry)
	{
		int idx = rand16() & (RBUF_COUNT - 1);
		if (cmpxchg64(remote_recv_buffer[target_lapic_id] + idx, 0, (uintptr_t)packet) == 0) break;
	}

	if (retry == IPE_RETRY)
	{
		lapic_ipi_issue(target_lapic_id);
		return -1;
	}

	return 0;
}
