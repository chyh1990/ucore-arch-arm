#ifndef __GLUE_INTR_H__
#define __GLUE_INTR_H__

/* Trap Numbers */

/* Processor-defined: */
#define T_RESET			0   // Reset
#define T_UNDEF			1   // Undefined instruction
#define T_SWI			2   // software interrupt
#define T_PABT			3   // Prefetch abort
#define T_DABT			4   // Data abort
// #define T_RESERVED
#define T_IRQ			6   // Interrupt request
#define T_FIQ			7   // Fast interrupt request

/* *
 * Hardware interrupt
 * ranges from 32 to 63
 * */
#define IRQ_OFFSET		32   // Interrupt request
#define IRQ_MAX_RANGE	63

/* *
 * These are arbitrarily chosen, but with care not to overlap
 * processor defined exceptions or interrupt vectors.
 * */
#define T_SWITCH_TOK            121 // a random system call
#define T_PANIC            122 // a random system call



#ifndef __ASSEMBLER__

#include <types.h>
#include <arch.h>
#include <intr.h>
#include <sync.h>


/* General purpose registers minus fp, sp and pc */
struct pushregs {
    uint32_t reg_r [13];
};

/* Trapframe structure 
 *   Structure built in the exception stack
 *   containing information on the nature of
 *   the occured exception.
 * */
struct trapframe {
	uint32_t tf_sr;			// saved status register
	uint32_t tf_trapno;		// Trap number
	uint32_t tf_err;		// Error code
	struct pushregs tf_regs;
	uint32_t tf_esp;		// esp
	uint32_t tf_epc;		// eip, actually lr
} __attribute__((packed));



#define local_intr_enable_hw  intr_enable()
#define local_intr_disable_hw intr_disable()

#define local_intr_save_hw(x)      local_intr_save(x)
#define local_intr_restore_hw(x)   local_intr_restore(x)

#endif /* !__ASSEMBLER__ */

#endif /* !__GLUE_INTR_H__ */
