#ifndef __KERN_DEBUG_KDEBUG_H__
#define __KERN_DEBUG_KDEBUG_H__

#include <types.h>

void print_kerninfo(void);
void print_stackframe(void);
void print_debuginfo(uintptr_t eip);

bool is_debugging();
void start_debug();
void end_debug();

#endif /* !__KERN_DEBUG_KDEBUG_H__ */

