#ifndef __LIBS_X86_RDTSC_H__
#define __LIBS_X86_RDTSC_H__

#include <libs/types.h>

static inline uint64_t rdtsc(void) __attribute__((always_inline));

static inline
uint64_t rdtsc(void)
{
	uint32_t lo, hi;
	// serialize
    __asm__ __volatile__ ("xorl %%eax,%%eax; cpuid"::: "%rax", "%rbx", "%rcx", "%rdx");
    /* We cannot use "=A", since this would use %rax on x86_64 and return only the lower 32bits of the TSC */
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return (uint64_t)hi << 32 | lo;
}

#endif
