#include <proc/dos.h>
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
#include <glue_intr.h>
#include <libs/string.h>
#include <glue_dos.h>

#define DOS_RETRY 10
#define DOS_REFRESH_INV 1

PLS static volatile char         *dos_buf;
PLS static int                    dos_buf_size;
PLS static volatile dosm_packet_t dos_packet_buf;
PLS static int                    dos_packet_buf_cap;
PLS static int                    dos_packet_local_head;
PLS static int                    dos_packet_local_last;

PLS static eproc_s dos_eproc;
PLS static event_s __dos_event;
PLS event_t dos_event = &__dos_event;
PLS static timer_s dos_timer;
PLS static int dos_enabled;

static void
dos_packet_handle(dosm_packet_t packet)
{
	event_activate((event_t)packet->priv);
}

static void
do_dos_scan(event_t e)
{
	int flag;
	
	local_intr_save_hw(flag);
	int end = dos_packet_local_last;
	int cur = dos_packet_local_head;
	local_intr_restore_hw(flag);

	int last = -1, next;
	while (cur != -1)
	{
		if (cur == end)
			next = -1;
		else
			next = dos_packet_buf[cur].offset_link;
		
		if (dos_packet_buf[cur].remote_flags & DOSM_RF_FINISHED)
		{
			dos_packet_handle(&dos_packet_buf[cur]);
			
			if (last != -1)
			{
				dos_packet_buf[last].offset_link = next;
			}
			else if (next != -1)
			{
				dos_packet_local_head = next;
			}
			else
			{
				local_intr_save_hw(flag);
				if (cur == dos_packet_local_last)
				{
					dos_packet_local_head = dos_packet_local_last = -1;
				}
				else dos_packet_local_head = dos_packet_buf[cur].offset_link;
				local_intr_restore_hw(flag);
			}
			memset(&dos_packet_buf[cur], 0, sizeof(dosm_packet_s));
			cur = last;
		}
		
		last = cur;
		cur = next;
	}

	if (e == &dos_timer.event)
	{
		timer_open(&dos_timer, timer_tick + DOS_REFRESH_INV * timer_freq);
	}
}

int
dos_init(void)
{
	dos_enabled = driver_os_is_enabled();
	if (!dos_enabled) return -1;
	
	dos_buf = driver_os_buffer_get();
	dos_buf_size = driver_os_buffer_size_get();
	dos_packet_buf = (dosm_packet_t)dos_buf;
	dos_packet_buf_cap = dos_buf_size / sizeof(dosm_packet_s);

	dos_packet_local_head = dos_packet_local_last = -1;
	
	eproc_open(&dos_eproc, "doscomm", (void(*)(void))proc_wait_try, NULL, 8192);
	event_open(dos_event, &dos_eproc.event_pool, do_dos_scan, NULL);
	event_open(&dos_timer.event, &dos_eproc.event_pool, do_dos_scan, NULL);
	timer_open(&dos_timer, timer_tick + DOS_REFRESH_INV * timer_freq);

	return 0;
}

static int
dos_packet_alloc(void)
{
	int retry;
	int idx;
	for (retry = 0; retry < DOS_RETRY; ++ retry)
	{
		idx = rand16() % dos_packet_buf_cap;
		if (cmpxchg16(&dos_packet_buf[idx].source_flags, 0, DOSM_SF_USED) == 0) break;
	}

	if (retry == DOS_RETRY)
	{
		driver_os_notify();
		return -1;
	}

	return idx;
}

int
dos_packet_send(event_t e, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5)
{
	if (!dos_enabled) return -1;
	
	int idx = dos_packet_alloc();
	if (idx == -1) return -1;

	dos_packet_buf[idx].priv = (uint64_t)e;
	dos_packet_buf[idx].args[0] = arg0;
	dos_packet_buf[idx].args[1] = arg1;
	dos_packet_buf[idx].args[2] = arg2;
	dos_packet_buf[idx].args[3] = arg3;
	dos_packet_buf[idx].args[4] = arg4;
	dos_packet_buf[idx].args[5] = arg5;

	dos_packet_buf[idx].source_lapic = lapic_id;
	dos_packet_buf[idx].remote_flags = 0;

	int flag;
	local_intr_save_hw(flag);
	if (dos_packet_local_last != -1)
		dos_packet_buf[dos_packet_local_last].offset_link = idx;
	else
	{
		dos_packet_local_head = dos_packet_local_last = idx;
	}
	local_intr_restore_hw(flag);

	dos_packet_buf[idx].source_flags |= DOSM_SF_VALID;
	
	driver_os_notify();

	return 0;
}
	
int
dos_packet_sendv(event_t e, uint64_t *args)
{
	if (!dos_enabled) return -1;
	
	int idx = dos_packet_alloc();
	if (idx == -1) return -1;

	dos_packet_buf[idx].priv = (uint64_t)e;
	memmove(dos_packet_buf[idx].args, args, sizeof(uint64_t) * 6);

	dos_packet_buf[idx].source_lapic = lapic_id;
	dos_packet_buf[idx].remote_flags = 0;

	int flag;
	local_intr_save_hw(flag);
	if (dos_packet_local_last != -1)
		dos_packet_buf[dos_packet_local_last].offset_link = idx;
	else
	{
		dos_packet_local_head = dos_packet_local_last = idx;
	}
	local_intr_restore_hw(flag);

	dos_packet_buf[idx].source_flags |= DOSM_SF_VALID;

	driver_os_notify();

	return 0;
}
