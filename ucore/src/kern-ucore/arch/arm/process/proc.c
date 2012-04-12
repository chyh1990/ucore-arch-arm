/* 
 * Chen Yuheng 2012/3
 * 
 */

#include <proc.h>
#include <mmu.h>
#include <vmm.h>
#include <slab.h>
#include <trap.h>
#include <arch.h>
#include <unistd.h>
#include <stdio.h>
#include <kio.h>
#include <string.h>
#include <sched.h>
#include <assert.h>
#include <syscall.h>

/* ------------- process/thread mechanism design&implementation -------------
(an simplified Linux process/thread mechanism )
introduction:
  ucore implements a simple process/thread mechanism. process contains the independent memory sapce, at least one threads
for execution, the kernel data(for management), processor state (for context switch), files(in lab6), etc. ucore needs to
manage all these details efficiently. In ucore, a thread is just a special kind of process(share process's memory).
------------------------------
process state       :     meaning               -- reason
    PROC_UNINIT     :   uninitialized           -- alloc_proc
    PROC_SLEEPING   :   sleeping                -- try_free_pages, do_wait, do_sleep
    PROC_RUNNABLE   :   runnable(maybe running) -- proc_init, wakeup_proc, 
    PROC_ZOMBIE     :   almost dead             -- do_exit

-----------------------------
process state changing:
                                            
  alloc_proc                                 RUNNING
      +                                   +--<----<--+
      +                                   + proc_run +
      V                                   +-->---->--+ 
PROC_UNINIT -- proc_init/wakeup_proc --> PROC_RUNNABLE -- try_free_pages/do_wait/do_sleep --> PROC_SLEEPING --
                                           A      +                                                           +
                                           |      +--- do_exit --> PROC_ZOMBIE                                +
                                           +                                                                  + 
                                           -----------------------wakeup_proc----------------------------------
-----------------------------
process relations
parent:           proc->parent  (proc is children)
children:         proc->cptr    (proc is parent)
older sibling:    proc->optr    (proc is younger sibling)
younger sibling:  proc->yptr    (proc is older sibling)
-----------------------------
related syscall for process:
SYS_exit        : process exit,                           -->do_exit
SYS_fork        : create child process, dup mm            -->do_fork-->wakeup_proc
SYS_wait        : wait process                            -->do_wait
SYS_exec        : after fork, process execute a program   -->load a program and refresh the mm
SYS_clone       : create child thread                     -->do_fork-->wakeup_proc
SYS_yield       : process flag itself need resecheduling, -- proc->need_sched=1, then scheduler will rescheule this process
SYS_sleep       : process sleep                           -->do_sleep 
SYS_kill        : kill process                            -->do_kill-->proc->flags |= PF_EXITING
                                                                 -->wakeup_proc-->do_wait-->do_exit   
SYS_getpid      : get the process's pid

*/


#define current (pls_read(current))

static void forkret();
//trap/entry.S
extern void forkrets(struct trapframe*tf);

void switch_to(struct context *from, struct context *to);

// alloc_proc - create a proc struct and init fields
struct proc_struct *
alloc_proc(void) {
    struct proc_struct *proc = kmalloc(sizeof(struct proc_struct));
    if (proc != NULL) {
        proc->state = PROC_UNINIT;
        proc->pid = -1;
        proc->runs = 0;
        proc->kstack = 0;
        proc->need_resched = 0;
        proc->parent = NULL;
        proc->mm = NULL;
        memset(&(proc->context), 0, sizeof(struct context));
        proc->tf = NULL;
        proc->cr3 = boot_pgdir_pa;
        proc->flags = 0;
        memset(proc->name, 0, PROC_NAME_LEN);
        proc->wait_state = 0;
        proc->cptr = proc->optr = proc->yptr = NULL;
        list_init(&(proc->thread_group));
        proc->rq = NULL;
        list_init(&(proc->run_link));
        proc->time_slice = 0;
        proc->sem_queue = NULL;
        event_box_init(&(proc->event_box));
        proc->fs_struct = NULL;
    }
    return proc;
}

// forkret -- the first kernel entry point of a new thread/process
// NOTE: the addr of forkret is setted in copy_thread function
//       after switch_to, the current proc will execute here.
static void
forkret(void) {
//	kprintf("forkret:tf to restore, %08x\n", current->tf->tf_epc);
	//print_trapframe(current->tf);
  forkrets(current->tf);
}


// kernel_thread - create a kernel thread using "fn" function
// NOTE: the trapframe is in the kernel stack of idle kernel thread
//       we should notice not to create more kernel thread, or the 
//       stack will be overfolw???
int
kernel_thread(int (*fn)(void *), void *arg, uint32_t clone_flags) {
  struct trapframe tf_struct;
  memset(&tf_struct, 0, sizeof(struct trapframe));
  tf_struct.tf_regs.reg_r[0] = (uint32_t)arg;
  tf_struct.tf_regs.reg_r[1] = (uint32_t)fn; // address to jump to (fn) is in r1, arg is in r0
  tf_struct.tf_epc = (uint32_t)kernel_thread_entry; // look at entry.S
  asm volatile ("mrs %0, cpsr" : "=r" (tf_struct.tf_sr)); // get spsr to be restored
  //kprintf("kernel_thread: writing spsr %08x\n", tf_struct.tf_sr);
  return do_fork(clone_flags | CLONE_VM, 0, &tf_struct);
}

void
de_thread_arch_hook (struct proc_struct *proc) {
}

int
do_execve_arch_hook (int argc, char** kargv) {
	return 0;
}

// copy_thread - setup the trapframe on the  process's kernel stack top and
//             - setup the kernel entry point and stack of process
int
copy_thread(uint32_t clone_flags, struct proc_struct *proc, 
    uintptr_t esp, struct trapframe *tf) {
  //~ kprintf("copy_thread: source %s sp:%08x tf:%08x\n", proc->name, esp, tf);
  proc->tf = (struct trapframe *)(proc->kstack + KSTACKSIZE) - 1;
 // kprintf("copy_thread: src %s copy stack:%08x-%08x tf:%08x\n", proc->name, proc->kstack, proc->kstack + KSTACKSIZE, proc->tf);
  //print_trapframe(tf);
  //kprintf("..................\n");
  *(proc->tf) = *tf;
  // the return value of a fork in child
  proc->tf->tf_regs.reg_r[0] = 0;
  //proc->tf->tf_regs.reg_r[11] = proc->tf; // fp
  proc->tf->tf_esp = esp;
  //proc->tf->tf_eflags |= FL_IF; // set interrupt flag

  /*kprintf("copy_thread:tf\n");
    print_trapframe(tf);
    kprintf("copy_thread:proc->tf\n");
    print_trapframe(proc->tf);*/

  // context has its value changed
  // setting sp to a trapframe, located on stack
  proc->context.epc = (uintptr_t)forkret;
  proc->context.esp = (uintptr_t)(proc->tf);
    return 0;
}

int
init_new_context (struct proc_struct *proc, struct elfhdr *elf, int argc, char** kargv) 
{
  uintptr_t stacktop = USTACKTOP - argc * PGSIZE;
  char **uargv = (char **)(stacktop - argc * sizeof(char *));
  int i;
  for (i = 0; i < argc; i ++) {
    uargv[i] = (char*) strcpy((char *)(stacktop + i * PGSIZE), kargv[i]);
  }
  stacktop = (uintptr_t)uargv;
  //*(int *)stacktop = argc;

  struct trapframe *tf = proc->tf;
  memset(tf, 0, sizeof(struct trapframe));
  tf->tf_esp = stacktop;
  tf->tf_epc = elf->e_entry;
  tf->tf_sr = ARM_SR_I|ARM_SR_MODE_USR; //user mode, interrput
	/* r3 = argc, r1 = argv 
   * initcode in user mode should copy r3 to r0
   */
	tf->tf_regs.reg_r[3] = argc;
	tf->tf_regs.reg_r[1] = (uintptr_t)uargv;

  //panic("UNIMPLEMENTED");
	return 0;
}



// kernel_execve - do SYS_exec syscall to exec a user program called by user_main kernel_thread
int
kernel_execve(const char *name, const char **argv) {
	//kprintf("kernel_execve: %s at 0x%08x-0x%08x\n", name, binary, size);
  int argc = 0, ret;
	while (argv[argc] != NULL) {
		argc ++;
	}
	//panic("unimpl");
  asm volatile (
  "mov\tr0,%1\n\t"
  "mov\tr1,%2\n\t"          
  "mov\tr2,%3\n\t"
  __syscall(exec) 
  "mov\t%0,r0"                                                        
        : "=r" (ret)
        : "r" ((long)(name)),"r" ((long)(argc)),"r" ((long)(argv))
        : "r0","r1","r2","lr");                    
   //ret = do_execve(name, argc, argv);
    return ret;
}


// cpu_idle - at the end of kern_init, the first kernel thread idleproc will do below works
void
cpu_idle(void) {
	kprintf("cpu_idle: starting\n");
    while (1) {
        if (current->need_resched) {
            //kprintf("*");
            schedule();
            //kprintf("!%d",current->pid);
        }
    }
}

