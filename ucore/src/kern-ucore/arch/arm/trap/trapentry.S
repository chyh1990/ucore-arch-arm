#modified by Chen Yuheng 2012/3

#include <glue_intr.h>

# vectors.S sends all traps here.
.text
.global __switrap
__switrap:
#TODO
	# Mode: SWI, I: disabled
	
	# link register offset don't change
	# Only valid for SWI
	
	# store context
	# Beware of stack overflow
	stmfd sp!, {lr}
	mrs r14, spsr
	and r14, r14, #0xF
	cmp r14, #0
	bleq __save_user
	ldr lr, [sp]
	sub sp, sp, #4
	stmfd sp!, {r0-r12}

	# Error code
	# Retrieve SWI number - SWI instruction AND NOT (0xff000000)
	LDR r0, [lr, #-4]
	# Mask off top 8 bits
	bic r0, r0, #0xff000000
	stmfd sp!, {r0}
	
  ldr r0, =T_SWI
	# Put trap number in stack (T_SWI)
	# Trap_no
	stmfd sp!, {r0}
	
	bl __alltraps
	
	b __trapret

.global __irqtrap
__irqtrap:
	# Mode: IRQ, I: disabled
	
	# link register offset
	# Only valid for IRQ/FIQ
	sub r14, r14, #4
	
	# store context
	# Beware of stack overflow
	stmfd sp!, {lr}
	mrs r14, spsr
	and r14, r14, #0xF
	cmp r14, #0
	bleq __save_user
	sub sp, sp, #4
	stmfd sp!, {r0-r12}
	
	# Error code
	mov r0, #0
	stmfd sp!, {r0}
	
	# Retrieve interrupt source
	# Put trap number in stack (32 + INT_source)
	ldr r0, =T_IRQ
	# Trap_no
	stmfd sp!, {r0}
	
	bl __alltraps
	
	b __trapret
	
.global __pretrap
__pretrap:
	# Mode: Prefetch abort, I: disabled
	
	# link register offset
	# Only valid for Prefetch abort
	sub r14, r14, #4
	
	# Note: this is the address causing a fault
	# Write it in far
	MCR p15, 0, lr, c6, c0, 0
	
	# store context
	# Beware of stack overflow
	stmfd sp!, {lr}
	mrs r14, spsr
	and r14, r14, #0xF
	cmp r14, #0
	bleq __save_user
	sub sp, sp, #4
	stmfd sp!, {r0-r12}
	
	# Error code: fsr for prefetch
	MRC p15, 0, r4, c5, c0, 1
	stmfd sp!, {r4}
	
	# Put Prefetch number
	mov r0, #T_PABT
	# Trap_no
	stmfd sp!, {r0}
	
	bl __alltraps
	
	b __trapret

.global __dattrap
__dattrap:
	# Mode: Data abort, I: disabled
	
	# link register offset
	# Only valid for Data abort
	sub r14, r14, #8
	
	# store context
	# Beware of stack overflow
	stmfd sp!, {lr}
	mrs r14, spsr
	and r14, r14, #0xF
	cmp r14, #0
	bleq __save_user
	sub sp, sp, #4
	stmfd sp!, {r0-r12}
	
	# Early abort
	mov		r6, r14
	mrs 	r7, spsr
	mrc     p15, 0, r4, c5, c0, 0           @ get FSR
	mrc     p15, 0, r5, c6, c0, 0           @ get FAR
	ldreq   r7, [r6]                        @ read aborted ARM instruction
	bic     r4, r4, #1 << 11 | 1 << 10      @ clear bits 11 and 10 of FSR
	tst     r7, #1 << 20                    @ check write
	orreq   r4, r4, #1 << 11
	
	# Error code: modified fsr
	stmfd sp!, {r4}
	
	# Put Prefetch number
	mov r0, #T_DABT
	stmfd sp!, {r0}
	
	bl __alltraps
	
	b __trapret

# common part of all exceptions, entry
.globl __alltraps
__alltraps:
	# Put spsr (saved state) in trapframe
	mrs r0, spsr
	stmfd sp!, {r0}
	
	# Put in r0 the adress of the stack
	# to pass a pointer to the trapframe as an argument to trap()
	mov r0, sp

	b trap


# common part of all exceptions, exit
.globl __trapret
__trapret:
	# Get rid of spsr, trap number, error code
	# to prepare for the context restoration
	ldmfd sp!,{r0-r2}
	
	# Either spsr is the supervisor mode, so we restore the registers excluding sp
	# or if spsr is the user mode, we update the whole range of registers including sp
  mov r1, r0
	and r0, r0, #0xF
	# in case spsr is user mode, r0 should be null
	cmp r0, #0
	beq __restore_user
	
	# Exiting the exception handler
	# restoring the registers at the state before exception
	# valid for every "non user" mode
	ldmfd sp!,{r0-r12}
	
	# discard esp value
	add sp, sp, #4
	
	# CPSR is automatically restored
	ldmfd sp!,{pc}^

__restore_user:
  msr spsr_c, r1
	# restoring the registers to the user banks, including sp
	ldmfd sp,{r0-r13}^
	add sp, sp, #56

  #we may modify rs in trap(privilege switch), so load cpsr from stack, r1
	ldmfd sp!,{pc}^
  


# .align 4
_memo:	
	.word	_memo

__save_user:
	# save sp from user bank to stack
	stmfd sp,{sp}^
	# ldr r0,=_memo
	# ldr sp, [sp, #-4]
	# str sp, [r0]
	mov pc, lr

.globl forkrets
forkrets:
    # set stack to this new process's trapframe
	mov sp, r0
	
	ldmfd sp!,{r0-r2}
	# in case the saved register is supervisor, it may have been manually saved
	# load the register and reload the pc without ^
	# in case the saved register is user, load everything + sp
  mov r1, r0
	and r0, r0, #0xF
	cmp r0, #0
	beq __restore_user
	ldmfd sp!,{r0-r12}
	add sp, sp, #4
	ldmfd sp!,{pc}


.globl __abort
__abort:

