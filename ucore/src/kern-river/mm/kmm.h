#ifndef __RIVER_KMM_H__
#define __RIVER_KMM_H__

#include <libs/types.h>
#include <glue_intr.h>

int   kmm_init(void);
void *kalloc(size_t size);
void  kfree(void *ptr);
void  kmm_pgfault(struct trapframe *tf);

#endif
