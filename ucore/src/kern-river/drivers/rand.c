#include <drivers/rand.h>
#include <drivers/hpet.h>
#include <glue_intr.h>
#include <glue_mp.h>

PLS static uint32_t seed;

int
rand_init(void)
{
	seed = *hpet_tick;
	return 0;
}

uint16_t
rand16(void)
{
	uint16_t r;
	int flag;
	local_intr_save_hw(flag);
	r = seed = seed * 1103515245 + 12345;
	local_intr_restore_hw(flag);
	return r;
}
