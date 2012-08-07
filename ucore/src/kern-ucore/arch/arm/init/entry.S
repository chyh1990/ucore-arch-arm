#
# Chen Yuheng 2012/3
#

#include <mmu.h>
#include <memlayout.h>
#include <board.h>


#if (SDRAM0_START & 0x001fffff)
#error "SDRAM0_START must be at an even 2MiB boundary!"
#endif


.text
.equ DISABLE_IRQ, 0x80
.equ DISABLE_FIQ, 0x40
.equ SYS_MOD, 0x1f
.equ IRQ_MOD, 0x12
.equ FIQ_MOD, 0x11
.equ SVC_MOD, 0x13
.equ ABT_MOD, 0x17
.equ UND_MOD, 0x1b

.equ MEM_SIZE, SDRAM0_SIZE 
.equ TEXT_BASE, SDRAM0_START

# assuming:
# MMU off, D-cache off

.globl kern_entry
kern_entry:
  mrs r0, cpsr
	bic r0, r0, #(DISABLE_FIQ|DISABLE_IRQ)
	msr cpsr, r0

# Stack inititialization - starts in SVC mode
	msr cpsr_c,#(DISABLE_IRQ|DISABLE_FIQ|IRQ_MOD)
	ldr sp,=irq_stacktop
	msr cpsr_c,#(DISABLE_IRQ|DISABLE_FIQ|FIQ_MOD)
	ldr sp,=fiq_stacktop
	msr cpsr_c,#(DISABLE_IRQ|DISABLE_FIQ|ABT_MOD)
	ldr sp,=abt_stacktop
	msr cpsr_c,#(DISABLE_IRQ|DISABLE_FIQ|UND_MOD)
	ldr sp,=und_stacktop
	msr cpsr_c,#(DISABLE_IRQ|DISABLE_FIQ|SYS_MOD)
	ldr sp,=sys_stacktop
	msr cpsr_c,#(DISABLE_IRQ|DISABLE_FIQ|SVC_MOD)

#in SVC mode
relocated:
    ldr sp,=bootstacktop

#if 0
    # enable the FPU
    mrc p15, 0, r0, c1, c0, 2
    orr r0, r0, #0x300000            /* single precision */
    orr r0, r0, #0xC00000            /* double precision */
    mcr p15, 0, r0, c1, c0, 2
    mov r0, #0x40000000
    fmxr fpexc,r0
#endif

/* enable swp,flow prediction */
#ifdef __MACH_ARM_ARMV7
  ldr r1, =(1<<11)|(1<<10)
  MCR p15, 0, r1, c1, c0, 0
#endif

# now kernel stack is ready , call the first C function
    b kern_init

# should never get here
    spin:
    b spin

    .data
# There might be a alignment problem, as should be aligned to a page size

    irq_stack:
    .space KSTACKSIZE
    irq_stacktop:
fiq_stack:
.space KSTACKSIZE
fiq_stacktop:
abt_stack:
.space KSTACKSIZE
abt_stacktop:
und_stack:
.space KSTACKSIZE
und_stacktop:
sys_stack:
    .space KSTACKSIZE
sys_stacktop:
    .globl bootstack
bootstack:
    .space KSTACKSIZE
    .globl bootstacktop
bootstacktop:
