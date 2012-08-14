/*
 * =====================================================================================
 *
 *       Filename:  arm_iomap_glue.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/21/2012 01:33:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */

#include <linux/kernel.h>
#include <linux/init.h>

#include <mach/hardware.h>
#include <asm/io.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/flash.h>
#include <asm/mach/map.h>
#include <asm/mach/time.h>

#define __NO_UCORE_TYPE__
#include <memlayout.h>
#include <pmm_glue.h>

volatile uint64_t jiffies_64;
unsigned long volatile jiffies;

extern uintptr_t *boot_pgdir;
void __init iotable_init(struct map_desc *io_desc, int nr)
{
  int i;
  for(i = 0;i < nr; i++){
    __boot_map_iomem(io_desc[i].virtual, io_desc[i].length, __pfn_to_phys(io_desc[i].pfn));
  }
}


extern struct machine_desc __arch_info_begin[], __arch_info_end[];
void dde_call_mapio_early()
{
  struct machine_desc *desc = &__arch_info_begin[0];
  if(__arch_info_end <= __arch_info_begin)
    return;
  printk("Init map_io: %s\n", desc->name);
  if(__arch_info_begin[0].map_io)
    __arch_info_begin[0].map_io();
}

void dde_call_machine_init()
{
  struct machine_desc *desc = &__arch_info_begin[0];
  if(__arch_info_end <= __arch_info_begin)
    return;
  printk("Init Machine: %s\n", desc->name);
  if(__arch_info_begin[0].init_machine)
    __arch_info_begin[0].init_machine();
}


/* dma for arm */
void *dma_alloc_writecombine(struct device *dev, size_t size, dma_addr_t *handle, gfp_t gfp)
{
  printk(KERN_ALERT "dma_alloc_writecombine size %08x\n", size);
  void *cpuaddr = ucore_kva_alloc_pages( (size+PAGE_SIZE-1)/PAGE_SIZE ,UCORE_KAP_IO);
  *handle = cpuaddr;
  return cpuaddr;
}

void __iomem *  
__arm_ioremap(unsigned long phys_addr, size_t size, unsigned int mtype)
{
  printk(KERN_INFO "ioremap %08x %08x \n", phys_addr, size);
  return __ucore_ioremap(phys_addr, size, mtype);
}

#define LPS_PREC 8
unsigned long loops_per_jiffy = (1<<12);

void __init calibrate_delay(void)
{
	unsigned long ticks, loopbit;
	int lps_precision = LPS_PREC; 

	loops_per_jiffy = (1<<12);

	printk("Calibrating delay loop... ");

	while (loops_per_jiffy <<= 1) {
		/* wait for "start of" clock tick */
		ticks = jiffies;
		while (ticks == jiffies)
			/* nothing */;
		/* Go .. */
		ticks = jiffies;
		__delay(loops_per_jiffy);
		ticks = jiffies - ticks;
		if (ticks)
			break;
	}

/* Do a binary approximation to get loops_per_jiffy set to equal one clock
   (up to lps_precision bits) */

	loops_per_jiffy >>= 1;
	loopbit = loops_per_jiffy; 
	while ( lps_precision-- && (loopbit >>= 1) ) {
		loops_per_jiffy |= loopbit;
		ticks = jiffies;
		while (ticks == jiffies);
		ticks = jiffies;
		__delay(loops_per_jiffy);
		if (jiffies != ticks)	/* longer than 1 tick */
			loops_per_jiffy &= ~loopbit;
	}

/* Round the value and print it */	
	printk("%lu.%lu BogoMIPS, loops_per_jiffy=%d\n",
		loops_per_jiffy/(500000/HZ),
		(loops_per_jiffy/(5000/HZ)) % 100, loops_per_jiffy);
}
