#ifndef __RIVER_OBJ_OBJECT_H__
#define __RIVER_OBJ_OBJECT_H__

#include <proc/event.h>

#define OBJECT_TYPE_NAME_MAX 32

typedef struct object_type_s *object_type_t;
typedef struct object_type_s
{
	const char name[OBJECT_TYPE_NAME_MAX];
} object_desc_s;

typedef struct object_head_s *object_head_t;
typedef struct object_head_s
{
	object_type_t type;
	object_ref_s *head;
	event_s       destroy_event;
} object_head_s;

void  object_init(void);

void *object_alloc(size_t size);
void  object_free(void *object);
void  object_protect(void *object);
void  object_unprotect(void *object);

#endif
