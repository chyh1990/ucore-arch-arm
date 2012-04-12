#ifndef __KERN_DRIVER_CLOCK_H__
#define __KERN_DRIVER_CLOCK_H__

#include <types.h>

extern volatile size_t ticks;

void clock_init(void);
//clear interrupt output after handling
void clock_clear(void);

#endif /* !__KERN_DRIVER_CLOCK_H__ */

