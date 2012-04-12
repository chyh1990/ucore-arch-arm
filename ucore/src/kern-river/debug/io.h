#ifndef __RIVER_DEBUGIO_H__
#define __RIVER_DEBUGIO_H__

#include <libs/types.h>
#include <libs/stdarg.h>

int kprintf(const char *fmt, ...);
int vkprintf(const char *fmt, va_list ap);

#endif
