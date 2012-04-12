#include <mp/mp.h>

PLS int lapic_id;
PLS int lcpu_idx;
PLS int lcpu_count;
PLS uint64_t lcpu_freq;

int mpconf_main_lcpu_idx;

int
mp_init(void)
{
	lapic_id = lapic_id_get();
	lcpu_idx = lcpu_idx_get();
	lcpu_count = lcpu_count_get();

	mpconf_main_lcpu_idx = 0;

	return 0;
}
