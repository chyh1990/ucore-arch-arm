#ifndef __RIVER_TRAP_H__
#define __RIVER_TRAP_H__

void local_irq_save(void);
void local_irq_restore(void);

int trap_init(void);

#include <libs/types.h>
typedef void(*irq_handler_f)(int irq_no, uint64_t accumulate);

void trap_irq_handler_set(int irq_no, irq_handler_f handler);

#endif
