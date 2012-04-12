#ifndef __LIBS_CRADIX_HEAP_H__
#define __LIBS_CRADIX_HEAP_H__

#include <libs/types.h>

/* CRADIX HEAP - The critial data structure for implement the timer mechanism */

#define CRH_KEY_BITSIZE         (64)
#define CRH_RADIX_BITSIZE_SHIFT (2)
#define CRH_RADIX_BITSIZE       (1 << CRH_RADIX_BITSIZE_SHIFT)
#define CRH_SEG_COUNT           (16)

typedef uint64_t crh_key_t;

typedef struct crh_node_s *crh_node_t;
typedef struct crh_node_s
{
	 crh_key_t  key;
	 crh_node_t next, prev;
} crh_node_s;

typedef struct crh_s *crh_t;
typedef struct crh_s
{
	 crh_key_t  base;
	 
	 uint64_t   bm;
	 uint64_t   heads_bm[CRH_SEG_COUNT];
	 
	 crh_node_t heads[CRH_SEG_COUNT][1 << CRH_RADIX_BITSIZE];
} crh_s;

void       crh_init(crh_t crh);
crh_node_t crh_set_base(crh_t crh, crh_key_t base);
crh_key_t  crh_max_step(crh_t crh);
int        crh_insert(crh_t crh, crh_node_t node);
void       crh_remove(crh_t crh, crh_node_t node);

#endif
