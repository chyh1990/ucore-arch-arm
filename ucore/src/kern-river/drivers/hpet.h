#ifndef __RIVER_HPET_H__
#define __RIVER_HPET_H__

#include <libs/types.h>

extern volatile const uint64_t *hpet_tick;
extern uint64_t                 hpet_tick_freq;

int hpet_init(void);

#endif
