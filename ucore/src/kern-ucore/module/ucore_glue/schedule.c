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


#define _TODO_() printk(KERN_ALERT "TODO %s\n", __func__)
//#define _TODO_()

/* schedule */
int wake_up_process(struct task_struct *tsk)
{
  _TODO_();
  return 0;
}

void async_synchronize_full(void)
{
  _TODO_();
}

void msleep(unsigned int msecs)
{
  _TODO_();
}

unsigned long msleep_interruptible(unsigned int msecs)
 {
  _TODO_();
   return msecs;
 }

void __wake_up(wait_queue_head_t *q, unsigned int mode, int nr, void *key)
{
  _TODO_();
}

int autoremove_wake_function(wait_queue_t *wait, unsigned mode, int sync, void *key)
{
  _TODO_();
  return 0;
}
void prepare_to_wait(wait_queue_head_t *q, wait_queue_t *wait, int state)
{
  _TODO_();
}

void finish_wait(wait_queue_head_t *q, wait_queue_t *wait)
{
  _TODO_();
}

void init_waitqueue_head(wait_queue_head_t *q)
{
  _TODO_();
}

signed long schedule_timeout(signed long timeout)
{
  _TODO_();
  return 0;
}

int schedule_work(struct work_struct *work)
{
  int ret = 1;
  pr_debug("TODO schedule_work()\n");
  work_func_t f = work->func;
  f(work);
  return 0;
}

pid_t pid_vnr(struct pid *pid)
{
  return -1;
}

int _cond_resched(void)
{
  //No need to implement
  return 0;
}

void wait_for_completion(struct completion *x)
{
  _TODO_();
}

/**
 * complete: - signals a single thread waiting on this completion
 * @x:  holds the state of this particular completion
 *
 * This will wake up a single thread waiting on this completion. Threads will be
 * awakened in the same order in which they were queued.
 *
 * See also complete_all(), wait_for_completion() and related routines.
 */
void complete(struct completion *x)
{
  _TODO_();
}

int default_wake_function(wait_queue_t *curr, unsigned mode, int sync,
    void *key)
{
  _TODO_();
}

