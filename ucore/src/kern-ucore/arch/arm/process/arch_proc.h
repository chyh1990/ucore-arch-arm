#ifndef __ARCH_ARM_INCLUDE_ARCH_PROC_H__
#define __ARCH_ARM_INCLUDE_ARCH_PROC_H__

#include <types.h>

// Saved registers for kernel context switches.
// Don't need to save all the %fs etc. segment registers,
// because they are constant across kernel contexts.
// Save all the regular registers so we don't need to care
// which are caller save, but not the return register %eax.
// (Not saving %eax just simplifies the switching code.)
// The layout of context must match code in switch.S.
// TODO: select relevant registers to keep
struct context {
  uint32_t epc; // r15
	uint32_t esp; // r13
	uint32_t er1; // r1
	uint32_t er2; // r2
	uint32_t er3; // r3
	uint32_t efp; // r11
	uint32_t eic; // r12
};

struct arch_proc_struct {
};

#endif /* !__KERN_PROCESS_PROC_H__ */

