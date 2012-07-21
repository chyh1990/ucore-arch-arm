/*
 * =====================================================================================
 *
 *       Filename:  schedule_glue.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/21/2012 02:50:09 PM
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
#include <linux/workqueue.h>
#include <linux/wait.h>

#define __NO_UCORE_TYPE__
#include <module.h>

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

int autoremove_wake_function(wait_queue_t *wait, unsigned mode, int sync, void *key)
{
  return 0;
}
void prepare_to_wait(wait_queue_head_t *q, wait_queue_t *wait, int state)
{
}

void finish_wait(wait_queue_head_t *q, wait_queue_t *wait)
{
}

void init_waitqueue_head(wait_queue_head_t *q)
{
}

signed long schedule_timeout(signed long timeout)
{
  return 0;
}

int schedule_work(struct work_struct *work)
{
  pr_debug("schedule_work()\n");
  return 0;
}

