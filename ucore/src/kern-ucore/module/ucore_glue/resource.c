/*
 * =====================================================================================
 *
 *       Filename:  resource.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/20/2012 08:32:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/device.h>
#include <linux/pfn.h>
#include <asm/io.h>

#if 0
/* resource */
struct resource ioport_resource = {
  .name   = "PCI IO",
  .start  = 0,
  .end    = IO_SPACE_LIMIT,
  .flags  = IORESOURCE_IO,
};

struct resource iomem_resource = {
  .name   = "PCI mem",
  .start  = 0,
  .end    = -1,
  .flags  = IORESOURCE_MEM,
};

int insert_resource(struct resource *parent, struct resource *new)
{
  printk(KERN_ALERT "TODO %s\n", __func__);
  return 0;
}

int release_resource(struct resource *old)
{
  printk(KERN_ALERT "TODO %s\n", __func__);
  return 0;
}
#endif
