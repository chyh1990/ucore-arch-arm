#include <proc/sched.h>

PLS static sched_node_s queue;

int
sched_init(void)
{
	queue.prev = queue.next = &queue;
	return 0;
}

int
sched_node_init(sched_node_t node)
{
	node->prev = node->next = node;
	return 0;
}

sched_node_t
sched_pick(void)
{
	if (queue.next == &queue) return NULL;
	else return queue.next;
}

int
sched_attach(sched_node_t node)
{
	node->next = &queue;
	node->prev = node->next->prev;

	node->next->prev = node;
	node->prev->next = node;
	return 0;
}

int
sched_detach(sched_node_t node)
{
	node->next->prev = node->prev;
	node->prev->next = node->next;
	return 0;
}
