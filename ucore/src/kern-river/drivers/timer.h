#ifndef __RIVER_TIMER_H__
#define __RIVER_TIMER_H__

#include <libs/crh.h>
#include <proc/event.h>

typedef struct timer_s *timer_t;
typedef void(*timer_cb_f)(timer_t timer);

typedef struct timer_s
{
	int        status;

	union
	{
		struct
		{
			event_s    event;
			crh_node_s crh_node;
		};

		timer_t *free_next;
	};
} timer_s;

void timer_open(timer_t timer, uint64_t tick);
void timer_close(timer_t timer);
int  timer_init(void);
void timer_measure(void);

extern volatile uint64_t timer_tick;
extern volatile uint64_t timer_freq;

#endif
