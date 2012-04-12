#ifndef __RIVER_MP_H__
#define __RIVER_MP_H__

#include <glue_mp.h>
#include <libs/types.h>

extern int lapic_id;
extern int lcpu_idx;
extern int lcpu_count;
extern int lcpu_river_count;
extern uint64_t lcpu_freq;

int mp_init(void);

#endif
