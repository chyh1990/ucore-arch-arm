#ifndef __RIVER_EVENT_H__
#define __RIVER_EVENT_H__

#include <libs/types.h>

typedef struct event_s      *event_t;
typedef struct event_pool_s *event_pool_t;

typedef void (*event_handler_f)      (event_t event);
typedef int  (*event_pool_touch_f)   (event_t event);
typedef void (*event_pool_handler_f) (event_pool_t pool);

typedef struct event_s
{
	event_pool_t    pool;
	uint32_t        status;
	event_handler_f handler;
	void           *private;

	event_t         next;
} event_s;

typedef struct event_pool_s
{
	event_t head;
	event_t tail;
	
	event_pool_touch_f   touch;
	event_pool_handler_f exhaust;
	int                  to_stop;
	event_pool_handler_f stop;
} event_pool_s;

void event_pool_init(event_pool_t pool,
					 event_pool_touch_f   touch,
					 event_pool_handler_f exhaust,
					 event_pool_handler_f stop);
void event_open(event_t event, event_pool_t pool, event_handler_f handler, void *private);
int  event_activate(event_t event);
void event_loop(event_pool_t pool);
void event_loop_stop(event_pool_t pool);

#endif
