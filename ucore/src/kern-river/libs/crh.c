#include <libs/crh.h>
#include <libs/string.h>
#include <libs/x86/bitsearch.h>

void crh_init(crh_t crh)
{
	 memset(crh, 0, sizeof(crh_s));
}

static inline int
_crh_insert(crh_t crh, crh_node_t node)
{
	 int d = bsr(crh->base ^ node->key) >> CRH_RADIX_BITSIZE_SHIFT;
	 int idx = (node->key >> (d << CRH_RADIX_BITSIZE_SHIFT)) & ((1 << CRH_RADIX_BITSIZE) - 1);

	 if ((node->next = crh->heads[d][idx]) != NULL)
		  node->next->prev = node;
	 node->prev = NULL;
	 crh->heads[d][idx] = node;

	 crh->heads_bm[d] |= 1 << idx;
	 crh->bm          |= 1 << d;

	 return d;
}

crh_node_t 
crh_set_base(crh_t crh, crh_key_t base)
{
	 if (crh->base == base) return NULL;
	 crh_node_t result;
	 result = NULL;
	 
	 int d = bsr(crh->base ^ base) >> CRH_RADIX_BITSIZE_SHIFT;
	 int idx = (base >> (d << CRH_RADIX_BITSIZE_SHIFT)) & ((1 << CRH_RADIX_BITSIZE) - 1);

	 crh->base = base;
	 crh_node_t node = crh->heads[d][idx];

	 crh->heads[d][idx] = NULL;
	 if (node != NULL)
	 {
		  if ((crh->heads_bm[d] ^= (1 << idx)) == 0)
			   crh->bm ^= 1 << d;
	 }

	 while (node != NULL)
	 {
		  crh_node_t next = node->next;
		  if (node->key != base)
		  {
			   _crh_insert(crh, node);
		  }
		  else
		  {
			   if (result == NULL)
			   {
					node->next = node->prev = node;
					result = node;
			   }
			   else
			   {
					node->next = result;
					node->prev = result->prev;
					node->next->prev = node;
					node->prev->next = node;
			   }
		  }
		  node = next;
	 }

	 return result;
}

int
crh_insert(crh_t crh, crh_node_t node)
{
	 if (crh->base == node->key) return -1;
	 _crh_insert(crh, node);
	 return 0;
}

crh_key_t crh_max_step(crh_t crh)
{
	 if (crh->bm == 0) return 0;
	 
	 int d = bsf(crh->bm);
	 int idx = (crh->base >> (d << CRH_RADIX_BITSIZE_SHIFT)) & ((1 << CRH_RADIX_BITSIZE) - 1);
	 crh_key_t remain = d == 0 ? 0 :
		 (crh->base & ((1 << (d << CRH_RADIX_BITSIZE_SHIFT)) - 1));
	 crh_key_t r;

	 if (d == CRH_SEG_COUNT - 1)
	 {
		  uint64_t r_bm = (crh->heads_bm[d] >> idx) |
			   (crh->heads_bm[d] << (CRH_RADIX_BITSIZE - idx));
		  r = (bsf(r_bm) << (d << CRH_RADIX_BITSIZE_SHIFT)) - remain;
	 }
	 else
	 {
		  uint64_t r_bm = (crh->heads_bm[d] >> idx);
		  if (r_bm == 0)
			   r = ((CRH_RADIX_BITSIZE - idx) << (d << CRH_RADIX_BITSIZE_SHIFT)) - remain;
		  else r = (bsf(r_bm) << (d << CRH_RADIX_BITSIZE_SHIFT)) - remain;
	 }
	 return r;

}

void
crh_remove(crh_t crh, crh_node_t node)
{
	 if (node->prev != NULL)
	 {
		  if ((node->prev->next = node->next) != NULL)
			   node->next->prev = node->prev;
		  return;
	 }

	 int d = bsr(crh->base ^ node->key) >> CRH_RADIX_BITSIZE_SHIFT;
	 int idx = (node->key >> (d << CRH_RADIX_BITSIZE_SHIFT)) & ((1 << CRH_RADIX_BITSIZE) - 1);
	 
	 if (node->next != NULL)
	 {
		  node->next->prev = NULL;
		  crh->heads[d][idx] = node->next;
	 }
	 else
	 {
		  crh->heads[d][idx] = NULL;
		  if ((crh->heads_bm[d] ^= 1 << idx) == 0)
		  {
			   crh->bm ^= 1 << d;
		  }
	 }
}
