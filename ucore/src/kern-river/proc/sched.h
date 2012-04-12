#ifndef __RIVER_SCHED_H__
#define __RIVER_SCHED_H__

#include <glue_mp.h>
#include <libs/types.h>

typedef struct sched_node_s *sched_node_t;
typedef struct sched_node_s
{
	sched_node_t prev, next;
} sched_node_s;

int sched_init(void);

int          sched_node_init(sched_node_t node);
/* All interfaces below should be used with interrupt disabled */
sched_node_t sched_pick(void);
int          sched_attach(sched_node_t node);
int          sched_detach(sched_node_t node);

#endif
