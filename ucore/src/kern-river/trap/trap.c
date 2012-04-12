#include <trap/trap.h>
#include <libs/types.h>
#include <libs/x86/bitsearch.h>
#include <glue_mp.h>
#include <glue_intr.h>
#include <proc/proc.h>
#include <mm/kmm.h>
#include <glue_memlayout.h>
#include <mp/mp.h>
#include <debug/io.h>
#include <proc/ipe.h>
#include <proc/dos.h>

PLS static uint64_t irq_mask;
PLS static uint64_t irq_accumulate[IRQ_COUNT];
PLS static irq_handler_f irq_handler[IRQ_COUNT];

static void
irq_process(void)
{
	int irq_no;
	irq_handler_f h;
	uint64_t acc;
	/* Assume irq is hw-disabled  */
	while (irq_mask)
	{
		irq_no = bsr(irq_mask);
		h      = irq_handler[irq_no];
		if (h)
		{
			acc = irq_accumulate[irq_no];
			
			irq_accumulate[irq_no] = 0;
			irq_mask ^= 1 << irq_no;

			local_intr_enable_hw;
			h(irq_no, acc);
			local_intr_disable_hw;
		}
		else
		{
			irq_mask ^= 1 << irq_no;
		}
	}
}

void
local_irq_save(void)
{
	int flag;
	local_intr_save_hw(flag);
	++ proc_current->irq_save_level;
	local_intr_restore_hw(flag);
}

void
local_irq_restore(void)
{
	int flag;
	local_intr_save_hw(flag);
	if (proc_current->irq_save_level == 1 && irq_mask)
	{
		irq_process();
		proc_schedule();
	}
	-- proc_current->irq_save_level;
	local_intr_restore_hw(flag);
}

static int
trap_in_kernel(struct trapframe *tf) {
    return (tf->tf_cs == (uint16_t)KERNEL_CS);
}

void
trap_dispatch(struct trapframe *tf)
{
	if (tf->tf_trapno < IRQ_COUNT)
	{
		/* EXCEPTION */
		switch (tf->tf_trapno)
		{
		case T_PGFLT:
			if (trap_in_kernel(tf))
			{
				kmm_pgfault(tf);
			}
			break;
		}
	}
	else if (tf->tf_trapno < IRQ_OFFSET + IRQ_COUNT)
	{
		/* Firstly acknowledge the IRQ */
		irq_ack(tf->tf_trapno - IRQ_OFFSET);
		
		/* IRQ */
		int irq_no = tf->tf_trapno - IRQ_OFFSET;
		irq_mask |= (1 << irq_no);
		++ irq_accumulate[irq_no];
		if (proc_current->irq_save_level == 0)
		{
			++ proc_current->irq_save_level;

			irq_process();
			proc_schedule();
			
			-- proc_current->irq_save_level;
		}
	}
	else
	{
		switch (tf->tf_trapno)
		{
		case T_SYSCALL:

			break;

		case T_IPI:
			event_activate(ipe_event);
			break;

		case T_IPI_DOS:
			event_activate(dos_event);
			break;
		}
	}
}

int
trap_init(void)
{
	int i;
	/* 32 == exceptions range in x86_64 */
	for (i = 0; i != 32; ++i)
	{
		intr_handler_set(i, trap_dispatch);
	}

	intr_handler_set(T_SYSCALL, trap_dispatch);
	intr_handler_set(T_IPI, trap_dispatch);
	intr_handler_set(T_IPI_DOS, trap_dispatch);

	irq_mask = 0;
	for (i = 0; i != IRQ_COUNT; ++ i)
	{
		irq_accumulate[i] = 0;
		irq_handler[i] = NULL;
	}

	return 0;
}

void
trap_irq_handler_set(int irq_no, irq_handler_f handler)
{
	int flag;
	local_intr_save_hw(flag);
	intr_handler_set(IRQ_OFFSET + irq_no, trap_dispatch);
	irq_handler[irq_no] = handler;
	if (irq_accumulate[irq_no]) irq_mask |= (1 << irq_no);
	local_intr_restore_hw(flag);
}
