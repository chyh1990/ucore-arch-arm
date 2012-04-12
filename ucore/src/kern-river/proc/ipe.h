#ifndef __RIVER_IPE_H__
#define __RIVER_IPE_H__

#include <libs/types.h>
#include <proc/event.h>

extern event_t ipe_event;

typedef struct ipe_packet_s *ipe_packet_t;
typedef void(*ipe_packet_handler_f)(ipe_packet_t packet);

typedef struct ipe_packet_s
{
	ipe_packet_handler_f handler;
	ipe_packet_handler_f back_handler;
	void                *private;

	int     processed;
	int     from_lapic;
} ipe_packet_s;

int  ipe_init(void);
void ipe_packet_init(ipe_packet_t packet, ipe_packet_handler_f handler, ipe_packet_handler_f back_handler, void *private);
int  ipe_packet_send(int lapic_id, ipe_packet_t packet);

#endif
