#ifndef __RIVER_OBJ_REF_H__
#define __RIVER_OBJ_REF_H__

typedef struct ref_s *ref_t;
typedef struct ref_s
{
	void *object;
	ref_t prev, next;
} object_ref_s;

void  ref_set(void *object);
void *ref_get(ref_t ref);

#endif
