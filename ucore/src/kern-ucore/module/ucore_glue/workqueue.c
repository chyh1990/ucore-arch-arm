/*
 * =====================================================================================
 *
 *       Filename:  workqueue.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/14/2012 11:39:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/signal.h>
#include <linux/completion.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/cpu.h>
#include <linux/notifier.h>
#include <linux/kthread.h>
#include <linux/hardirq.h>
#include <linux/mempolicy.h>
#include <linux/freezer.h>
#include <linux/kallsyms.h>
#include <linux/debug_locks.h>
#include <linux/lockdep.h>

#define _TODO_() printk(KERN_ALERT "TODO %s\n", __func__)

struct cpu_workqueue_struct {

        spinlock_t lock;

        struct list_head worklist;
        wait_queue_head_t more_work;
        struct work_struct *current_work;

        struct workqueue_struct *wq;
        struct task_struct *thread;
} ____cacheline_aligned;

/*
 * The externally visible workqueue abstraction is an array of
 * per-CPU workqueues:
 */
struct workqueue_struct {
        struct cpu_workqueue_struct *cpu_wq;
        struct list_head list;
        const char *name;
        int singlethread;
        int freezeable;         /* Freeze threads during suspend */
        int rt;
#ifdef CONFIG_LOCKDEP
        struct lockdep_map lockdep_map;
#endif
};

/* If it's single threaded, it isn't in the list of workqueues. */
static inline int is_wq_single_threaded(struct workqueue_struct *wq)
{
        return wq->singlethread;
}

int queue_delayed_work(struct workqueue_struct *wq,
      struct delayed_work *dwork, unsigned long delay)
{
  _TODO_();
  return -EINVAL;
}

void flush_workqueue(struct workqueue_struct *wq)
{
  _TODO_();
}

void add_wait_queue(wait_queue_head_t *q, wait_queue_t *wait)
{
  _TODO_();
}
/**
 * destroy_workqueue - safely terminate a workqueue
 * @wq: target workqueue
 *
 * Safely destroy a workqueue. All work currently pending will be done first.
898  */
void destroy_workqueue(struct workqueue_struct *wq)
{
  _TODO_();
}

void remove_wait_queue(wait_queue_head_t *q, wait_queue_t *wait)
{
  _TODO_();
}
EXPORT_SYMBOL(remove_wait_queue);

static int worker_thread(void *__cwq)
{
        struct cpu_workqueue_struct *cwq = __cwq;

        for (;;) {
                //run_workqueue(cwq);
                extern do_sleep(int);
                do_sleep(10);
        }

        return 0;
}


static int create_workqueue_thread(struct cpu_workqueue_struct *cwq, int cpu)
{
  struct workqueue_struct *wq = cwq->wq;
  const char *fmt = is_wq_single_threaded(wq) ? "%s" : "%s/%d";
  struct task_struct * p = kthread_create(worker_thread, cwq, fmt, wq->name, cpu);
  if (IS_ERR(p))
    return PTR_ERR(p);
  cwq->thread = p;
  return 0;
}

struct workqueue_struct *__create_workqueue_key(const char *name,
                                                int singlethread,
                                                int freezeable,
                                                int rt,
                                                struct lock_class_key *key,
                                                const char *lock_name)
{
  struct workqueue_struct *wq = NULL;
  int err = 0;
  if(!singlethread){
    pr_debug("__create_workqueue_key only support singlethread\n");
    return NULL;
  }
  wq = kzalloc(sizeof(struct workqueue_struct), GFP_KERNEL);
  if(!wq)
    return NULL;
  wq->cpu_wq = kzalloc(sizeof(struct cpu_workqueue_struct), GFP_KERNEL);
  if(!wq->cpu_wq){
    kfree(wq);
    return NULL;
  }
  wq->cpu_wq->wq = wq;

  wq->name = name;
  wq->singlethread = singlethread;
  wq->freezeable = freezeable;
  wq->rt = rt;
  err = create_workqueue_thread(wq->cpu_wq, 0);
  if(err)
    goto create_thread_failed;

  _TODO_();
  return wq;
create_thread_failed:
  kfree(wq->cpu_wq);
  kfree(wq);
  return NULL;
}

