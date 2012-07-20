/*
 * =====================================================================================
 *
 *       Filename:  module.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/17/2012 09:02:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */

#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/err.h>
#include <linux/gfp.h>
#include <linux/init.h>

#define __NO_UCORE_TYPE__
#include <module.h>
#include <kio.h>
#include <slab.h>
//#include <assert.h>


extern initcall_t __initcall_start[], __initcall_end[];

static void do_initcalls()
{
  int i;
  int cnt = __initcall_end - __initcall_start;
  kprintf("do_initcalls() %08x %08x %d calls, begin...\n", __initcall_start, __initcall_end,cnt);
  for(i=0;i < cnt; i++){
    kprintf("  calling 0x%08x\n", __initcall_start[i]);
    __initcall_start[i]();
  }
  kprintf("do_initcalls() end!\n");
}

void dde_init()
{
  do_initcalls();
  driver_init();
}

/* basic functions */
int printk(const char *s, ...)
{
    va_list ap;
    va_start(ap, s);
    int cnt = vkprintf(s, ap);
    va_end(ap);
    return cnt;
}

void panic(const char *fmt, ...)
{
  intr_disable();
  va_list ap;
  va_start(ap, fmt);
  kprintf("kernel panic in MODULE:\n    ");
  vkprintf(fmt, ap);
  kprintf("\n");
  va_end(ap);
}

void warn_slowpath(const char *file, int line, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  kprintf("kernel panic in %s:%d:\n    ", file, line);
  vkprintf(fmt, ap);
  kprintf("\n");
  va_end(ap);
}

void __attribute__((noreturn)) __bug(const char *file, int line)
{
         printk(KERN_CRIT"kernel BUG at %s:%d!\n", file, line);
         panic("0");
         /* Avoid "noreturn function does return" */
         for (;;);
}

int sprint_symbol(char *buffer, unsigned long address)
{
        char *modname;
        const char *name;
        unsigned long offset, size;
        int len;

        //name = kallsyms_lookup(address, &size, &offset, &modname, buffer);
        name = NULL;
        if (!name)
                return sprintf(buffer, "0x%lx", address);

        if (name != buffer)
                strcpy(buffer, name);
        len = strlen(buffer);
        buffer += len;

        if (modname)
                len += sprintf(buffer, "+%#lx/%#lx [%s]",
                                                offset, size, modname);
        else
                len += sprintf(buffer, "+%#lx/%#lx", offset, size);

        return len;
}

/* Look up a kernel symbol and print it to the kernel messages. */
void __print_symbol(const char *fmt, unsigned long address)
{
  printk("__print_symbol: %s, 0x%08x\n", fmt, address);
}

void __memzero(void *ptr, __kernel_size_t n)
{
  memset(ptr, 0, n);
}

void *__kmalloc(size_t size, gfp_t flags)
{
  void *ptr = kmalloc(size);
  if(flags | __GFP_ZERO){
    memset(ptr, 0, size);
  }
  return ptr;
}

char *kstrdup(const char *s, gfp_t gfp)
{
        size_t len;
        char *buf;

        if (!s)
                return NULL;

        len = strlen(s) + 1;
        buf = __kmalloc(len, gfp);
        if (buf)
                memcpy(buf, s, len);
        return buf;
}



/* mutux */
void __mutex_init(struct mutex *lock, const char *name, struct lock_class_key *key)
{
}

extern void __init_rwsem(struct rw_semaphore *sem, const char *name,
              struct lock_class_key *key)
{
}

void mutex_lock(struct mutex *lock)
{
  //TODO
}

void mutex_unlock(struct mutex *lock)
{
  //TODO
}

/* notifier */
int blocking_notifier_call_chain(struct blocking_notifier_head *nh,
  unsigned long val, void *v)
{
  return 0;
}

extern int blocking_notifier_chain_register(struct blocking_notifier_head *nh,
                 struct notifier_block *nb)
{
  return 0;
}

extern int blocking_notifier_chain_unregister(struct blocking_notifier_head *nh,
                 struct notifier_block *nb)
{
  return 0;
}

/* sysfs */
int sysfs_create_link(struct kobject *kobj, struct kobject *target,
                      const char *name)
{
  return 0;
}
int sysfs_create_dir(struct kobject *kobj)
{
  return 0;
}
void sysfs_remove_dir(struct kobject *kobj)
{
  return;
}
int sysfs_rename_dir(struct kobject *kobj, const char *new_name)
{
  return 0;
}
int sysfs_move_dir(struct kobject *kobj,
                       struct kobject *new_parent_kobj)
{
  return 0;
}

/**
 *      sysfs_create_link_nowarn - create symlink between two objects.
 *      @kobj:  object whose directory we're creating the link in.
 *      @target:        object we're pointing to.
 *      @name:          name of the symlink.
 *
 *      This function does the same as sysf_create_link(), but it
 *      doesn't warn if the link already exists.
 */
int sysfs_create_link_nowarn(struct kobject *kobj, struct kobject *target,
                             const char *name)
{
  return 0;
}

/**
 *      sysfs_remove_link - remove symlink in object's directory.
 *      @kobj:  object we're acting for.
 *      @name:  name of the symlink to remove.
 */

void sysfs_remove_link(struct kobject * kobj, const char * name)
{
}

int sysfs_create_file(struct kobject *kobj,
                                    const struct attribute *attr)
{
  return 0;
}

int sysfs_chmod_file(struct kobject *kobj, struct attribute *attr,
                                   mode_t mode)
{
  return 0;
}
void sysfs_remove_file(struct kobject *kobj, const struct attribute *attr)
{
  return;
}

int sysfs_create_group(struct kobject *kobj,
                                    const struct attribute_group *grp)
{
  return 0;
}

int sysfs_update_group(struct kobject *kobj,
                       const struct attribute_group *grp)
{
  return 0;
}

void sysfs_remove_group(struct kobject *kobj,
                        const struct attribute_group *grp)
{
}

int sysfs_create_bin_file(struct kobject *kobj,
    struct bin_attribute *attr)
{
  return 0;
}
void sysfs_remove_bin_file(struct kobject *kobj, struct bin_attribute *attr)
{
  return;
}

int sysfs_schedule_callback(struct kobject *kobj, void (*func)(void *),
                             void *data, struct module *owner)
{
  return 0;
}

int add_uevent_var(struct kobj_uevent_env *env, const char *format, ...)
{
  printk("INFO: add_uevent_var: %s\n", format);
  return 0;
}

int kobject_uevent(struct kobject *kobj, enum kobject_action action)
{
  return 0;
}

int kobject_uevent_env(struct kobject *kobj, enum kobject_action action,
                        char *envp[])
{
  return 0;
}

int kobject_action_type(const char *buf, size_t count,
                        enum kobject_action *type)
{
  return 0;
}

/* firmware */
int firmware_init(void)
{
  return 0;
}

/* cpu */
int cpu_dev_init()
{
  return 0;
}

/* dma */
int
dma_declare_coherent_memory(struct device *dev, dma_addr_t bus_addr,
                            dma_addr_t device_addr, size_t size, int flags)
{
        return 0;
}

void
dma_release_declared_memory(struct device *dev)
{
}

void *
dma_mark_declared_memory_occupied(struct device *dev,
                                  dma_addr_t device_addr, size_t size)
{
        return ERR_PTR(-EBUSY);
}

void *
 dma_alloc_coherent(struct device *dev, size_t size, dma_addr_t *dma_handle,
                    gfp_t flag)
 {
 //        return pci_alloc_consistent(to_pci_dev(dev), size, dma_handle);
    panic("TODO");
    return NULL;
 }
 
void
 dma_free_coherent(struct device *dev, size_t size, void *cpu_addr,
                     dma_addr_t dma_handle)
 {
 }

/* schedule */
int wake_up_process(struct task_struct *tsk)
{
  panic("TODO");
  return 0;
}

void async_synchronize_full(void)
{
}

void msleep(unsigned int msecs)
{
}

unsigned long msleep_interruptible(unsigned int msecs)
 {
   return msecs;
 }

void __wake_up(wait_queue_head_t *q, unsigned int mode, int nr, void *key){
}
