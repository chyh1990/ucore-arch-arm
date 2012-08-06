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

extern uintptr_t *boot_pgdir;
void __init iotable_init(struct map_desc *io_desc, int nr)
{
  int i;
  for(i = 0;i < nr; i++){
    __boot_map_iomem(io_desc[i].virtual, io_desc[i].length, __pfn_to_phys(io_desc[i].pfn));
  }
}


void dde_call_mapio_early()
{
  extern struct machine_desc __arch_info_begin[], __arch_info_end[];
  if(__arch_info_end <= __arch_info_begin)
    return;
  __arch_info_begin[0].map_io();
}

void dde_call_machine_init()
{
  extern struct machine_desc __arch_info_begin[], __arch_info_end[];
  if(__arch_info_end <= __arch_info_begin)
    return;
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
  return __ucore_ioremap(phys_addr, size, mtype);
}

