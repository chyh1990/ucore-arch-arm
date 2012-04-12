#include <proc/event.h>
#include <glue_mp.h>
#include <glue_intr.h>
#include <trap/trap.h>

PLS event_pool_s global_pool;

#define EVENT_STATUS_WAIT  0
#define EVENT_STATUS_QUEUE 1
#define EVENT_STATUS_QUEUE_STRONG 2

void
event_pool_init(event_pool_t pool,
				event_pool_touch_f   touch,
				event_pool_handler_f exhaust,
				event_pool_handler_f stop)
{
	if (pool == NULL)
		pool = &global_pool;

	pool->head = pool->tail = NULL;

	pool->touch = touch;
	pool->exhaust = exhaust;
	pool->to_stop = 0;
	pool->stop = stop;
}

void
event_open(event_t event, event_pool_t pool, event_handler_f handler, void *private)
{
	if (pool == NULL)
		pool = &global_pool;

	event->pool = pool;
	event->status = EVENT_STATUS_WAIT;
	event->handler = handler;
	event->private = private;
}

int
event_activate(event_t event)
{
	local_irq_save();
	
	event_pool_t pool = event->pool;
	if (pool->touch(event) == 0)
	{
		switch (event->status)
		{
		case EVENT_STATUS_WAIT:
		{
			event->status = EVENT_STATUS_QUEUE;
			event->next   = NULL;
			if (pool->tail == NULL)
			{
				pool->head = pool->tail = event;
			}
			else
			{
				pool->tail->next = event;
				pool->tail = event;
			}
			
			break;
		}
		
		case EVENT_STATUS_QUEUE:
			event->status = EVENT_STATUS_QUEUE_STRONG;
			break;
		}
	}
	
	local_irq_restore();
	return 0;
}

void
event_loop(event_pool_t pool)
{
	if (pool == NULL)
		pool = &global_pool;
	
	while (1)
	{
		event_t event;
		local_irq_save();
		if (pool->head == NULL)
		{
			if (pool->to_stop)
			{
				local_irq_restore();
				break;
			}
			event = NULL;
		}
		else
		{
			event = pool->head;
			switch (event->status)
			{
			case EVENT_STATUS_QUEUE:
				
				event->status = EVENT_STATUS_WAIT;
				if ((pool->head = pool->head->next) == NULL)
				{
					pool->tail = NULL;
				}

				break;

			case EVENT_STATUS_QUEUE_STRONG:

				event->status = EVENT_STATUS_QUEUE;
				if (pool->tail != event)
				{
					pool->head = pool->head->next;
					pool->tail->next = event;
					pool->tail = event;
					
					event->next = NULL;
				}

				break;
			}
				
		}
		local_irq_restore();

		if (event)
			event->handler(event);
		else pool->exhaust(pool);
	}

	pool->stop(pool);
}

void
event_loop_stop(event_pool_t pool)
{
	if (pool == NULL)
		pool = &global_pool;
	/* Need not irq disabled */
	pool->to_stop = 1;
}
