/*
 *  init.S 
 *  stack initialization
 *  supervisor mode
 *  jump to bootmain
 *
 *  author: YU Kuanlong / HE Alexis
 *  modified: Chen Yuheng
 *  date:   2012-3-21
 */

.text
.code 32
.align 0
.global _start
.extern __bss_start__
.extern __bss_end__

.equ DISABLE_IRQ, 0x80
.equ DISABLE_FIQ, 0x40
.equ SVC_MOD, 0x13

.equ _SVC_STACK, (BOOTLOADER_BASE-4096)

_start: # start address at cpu reset is 0x0: jump to _vector_reset

# Exception vector table
__vector_table:
	b  _vector_reset
	ldr pc,_vector_undefined
	ldr pc,_vector_swi
	ldr pc,_vector_prefetch_abort
	ldr pc,_vector_data_abort
	ldr pc,_vector_reserved
	ldr pc,_vector_irq
	ldr pc,_vector_fiq

# To be replaced by OS with actual adresses of handlers
# .align 4
_vector_reset:	
	.word	__vector_reset
_vector_undefined:	
	.word	__vector_undefined
_vector_swi:	
	.word	__vector_swi
_vector_prefetch_abort:	
	.word	__vector_prefetch_abort
_vector_data_abort:	
	.word	__vector_data_abort
_vector_reserved:	
	.word	__vector_reserved
_vector_irq:	
	.word	__vector_irq
_vector_fiq:	
	.word	__vector_fiq 

# Stack inititialization - starts in SVC mode
__vector_reset:
	msr cpsr_c,#(DISABLE_IRQ|DISABLE_FIQ|SVC_MOD)
	ldr sp,=_SVC_STACK

__vector_undefined:
__vector_swi:
__vector_prefetch_abort:
__vector_data_abort:
__vector_reserved:
__vector_irq:
__vector_fiq:
	
_boot:
	bl bootmain
#never return
	b	_start

