#ifndef __RIVER_DOS_H__
#define __RIVER_DOS_H__

#include <libs/types.h>
#include <proc/event.h>

extern event_t dos_event;

#include <dosm-packet.h>

int dos_init(void);
int dos_packet_send(event_t e, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5);
int dos_packet_sendv(event_t e, uint64_t *args);

#endif
