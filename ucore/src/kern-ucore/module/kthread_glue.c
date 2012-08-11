/*
 * =====================================================================================
 *
 *       Filename:  kthread_glue.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/11/2012 02:38:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */
#include <linux/kernel.h>
#include <linux/gfp.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/completion.h>
#include <linux/wakelock.h>

#define _TODO_() printk(KERN_ALERT "TODO %s\n", __func__)

struct task_struct *kthread_create(int (*threadfn)(void *data),
                                   void *data,
                                   const char namefmt[],
                                   ...)
{
  _TODO_();
  return NULL;
}

