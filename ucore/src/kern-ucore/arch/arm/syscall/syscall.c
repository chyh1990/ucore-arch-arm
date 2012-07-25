#include <proc.h>
#include <trap.h>
#include <stdio.h>
#include <pmm.h>
#include <clock.h>
#include <assert.h>
#include <sem.h>
#include <event.h>
#include <mbox.h>
#include <stat.h>
#include <dirent.h>
#include <sysfile.h>
#include <kio.h>
#include <error.h>
#include <syscall.h>


static uint32_t
sys_exit(uint32_t arg[]) {
    int error_code = (int)arg[0];
    return do_exit(error_code);
}

static uint32_t
sys_fork(uint32_t arg[]) {
    struct trapframe *tf = pls_read(current)->tf;
    uintptr_t stack = tf->tf_esp;
    return do_fork(0, stack, tf);
}

static uint32_t
sys_wait(uint32_t arg[]) {
    int pid = (int)arg[0];
    int *store = (int *)arg[1];
    return do_wait(pid, store);
}

static uint32_t
sys_execve(uint32_t arg[]) {
	const char *name = (const char *)arg[0];
	const char **argv = (const char **)arg[1];
	const char **envp = (const char **)arg[2];
    return do_execve(name, argv, envp);
}


static uint32_t
sys_clone(uint32_t arg[]) {
    struct trapframe *tf = pls_read(current)->tf;
    uint32_t clone_flags = (uint32_t)arg[0];
    uintptr_t stack = (uintptr_t)arg[1];
    if (stack == 0) {
        stack = tf->tf_esp;
    }
    return do_fork(clone_flags, stack, tf);
}

static uint32_t
sys_exit_thread(uint32_t arg[]) {
    int error_code = (int)arg[0];
    return do_exit_thread(error_code);
}

static uint32_t
sys_yield(uint32_t arg[]) {
    return do_yield();
}

static uint32_t
sys_kill(uint32_t arg[]) {
    int pid = (int)arg[0];
    return do_kill(pid, -E_KILLED);
}

static uint32_t
sys_getpid(uint32_t arg[]) {
    return pls_read(current)->pid;
}

static uint32_t
sys_sleep(uint32_t arg[]) {
    unsigned int time = (unsigned int)arg[0];
    return do_sleep(time);
}

static uint32_t
sys_gettime(uint32_t arg[]) {
    return (int)ticks;
}

static uint32_t
sys_putc(uint32_t arg[]) {
    int c = (int)arg[0];
    kcons_putc(c);
    return 0;
}

static uint32_t
sys_pgdir(uint32_t arg[]) {
    print_pgdir(kprintf);
    return 0;
}


static uint32_t
sys_brk(uint32_t arg[]) {
    uintptr_t *brk_store = (uintptr_t *)arg[0];
    return do_brk(brk_store);
}

static uint32_t
sys_mmap(uint32_t arg[]) {
    uintptr_t *addr_store = (uintptr_t *)arg[0];
    size_t len = (size_t)arg[1];
    uint32_t mmap_flags = (uint32_t)arg[2];
    return do_mmap(addr_store, len, mmap_flags);
}

static uint32_t
sys_munmap(uint32_t arg[]) {
    uintptr_t addr = (uintptr_t)arg[0];
    size_t len = (size_t)arg[1];
    return do_munmap(addr, len);
}

static uint32_t
sys_shmem(uint32_t arg[]) {
    uintptr_t *addr_store = (uintptr_t *)arg[0];
    size_t len = (size_t)arg[1];
    uint32_t mmap_flags = (uint32_t)arg[2];
    return do_shmem(addr_store, len, mmap_flags);
}


static uint32_t
sys_sem_init(uint32_t arg[]) {
    int value = (int)arg[0];
    return ipc_sem_init(value);
}

static uint32_t
sys_sem_post(uint32_t arg[]) {
    sem_t sem_id = (sem_t)arg[0];
    return ipc_sem_post(sem_id);
}

static uint32_t
sys_sem_wait(uint32_t arg[]) {
    sem_t sem_id = (sem_t)arg[0];
    unsigned int timeout = (unsigned int)arg[1];
    return ipc_sem_wait(sem_id, timeout);
}

static uint32_t
sys_sem_free(uint32_t arg[]) {
    sem_t sem_id = (sem_t)arg[0];
    return ipc_sem_free(sem_id);
}

static uint32_t
sys_sem_get_value(uint32_t arg[]) {
    sem_t sem_id = (sem_t)arg[0];
    int *value_store = (int *)arg[1];
    return ipc_sem_get_value(sem_id, value_store);
}

static uint32_t
sys_event_send(uint32_t arg[]) {
    int pid = (int)arg[0];
    int event = (int)arg[1];
    unsigned int timeout = (unsigned int)arg[2];
    return ipc_event_send(pid, event, timeout);
}

static uint32_t
sys_event_recv(uint32_t arg[]) {
    int *pid_store = (int *)arg[0];
    int *event_store = (int *)arg[1];
    unsigned int timeout = (unsigned int)arg[2];
    return ipc_event_recv(pid_store, event_store, timeout);
}

static uint32_t
sys_mbox_init(uint32_t arg[]) {
    unsigned int max_slots = (unsigned int)arg[0];
    return ipc_mbox_init(max_slots);
}

static uint32_t
sys_mbox_send(uint32_t arg[]) {
    int id = (int)arg[0];
    struct mboxbuf *buf = (struct mboxbuf *)arg[1];
    unsigned int timeout = (unsigned int)arg[2];
    return ipc_mbox_send(id, buf, timeout);
}

static uint32_t
sys_mbox_recv(uint32_t arg[]) {
    int id = (int)arg[0];
    struct mboxbuf *buf = (struct mboxbuf *)arg[1];
    unsigned int timeout = (unsigned int)arg[2];
    return ipc_mbox_recv(id, buf, timeout);
}

static uint32_t
sys_mbox_free(uint32_t arg[]) {
    int id = (int)arg[0];
    return ipc_mbox_free(id);
}

static uint32_t
sys_mbox_info(uint32_t arg[]) {
    int id = (int)arg[0];
    struct mboxinfo *info = (struct mboxinfo *)arg[1];
    return ipc_mbox_info(id, info);
}

static uint32_t
sys_open(uint32_t arg[]) {
    const char *path = (const char *)arg[0];
    uint32_t open_flags = (uint32_t)arg[1];
    return sysfile_open(path, open_flags);
}

static uint32_t
sys_close(uint32_t arg[]) {
    int fd = (int)arg[0];
    return sysfile_close(fd);
}

static uint32_t
sys_read(uint32_t arg[]) {
    int fd = (int)arg[0];
    void *base = (void *)arg[1];
    size_t len = (size_t)arg[2];
    return sysfile_read(fd, base, len);
}

static uint32_t
sys_write(uint32_t arg[]) {
    int fd = (int)arg[0];
    void *base = (void *)arg[1];
    size_t len = (size_t)arg[2];
    return sysfile_write(fd, base, len);
}

static uint32_t
sys_seek(uint32_t arg[]) {
    int fd = (int)arg[0];
    off_t pos = (off_t)arg[1];
    int whence = (int)arg[2];
    return sysfile_seek(fd, pos, whence);
}

static uint32_t
sys_fstat(uint32_t arg[]) {
    int fd = (int)arg[0];
    struct stat *stat = (struct stat *)arg[1];
    return sysfile_fstat(fd, stat);
}

static uint32_t
sys_fsync(uint32_t arg[]) {
    int fd = (int)arg[0];
    return sysfile_fsync(fd);
}

static uint32_t
sys_chdir(uint32_t arg[]) {
    const char *path = (const char *)arg[0];
    return sysfile_chdir(path);
}

static uint32_t
sys_getcwd(uint32_t arg[]) {
    char *buf = (char *)arg[0];
    size_t len = (size_t)arg[1];
    return sysfile_getcwd(buf, len);
}

static uint32_t
sys_mkdir(uint32_t arg[]) {
    const char *path = (const char *)arg[0];
    return sysfile_mkdir(path);
}

static uint32_t
sys_link(uint32_t arg[]) {
    const char *path1 = (const char *)arg[0];
    const char *path2 = (const char *)arg[1];
    return sysfile_link(path1, path2);
}

static uint32_t
sys_rename(uint32_t arg[]) {
    const char *path1 = (const char *)arg[0];
    const char *path2 = (const char *)arg[1];
    return sysfile_rename(path1, path2);
}

static uint32_t
sys_unlink(uint32_t arg[]) {
    const char *name = (const char *)arg[0];
    return sysfile_unlink(name);
}

static uint32_t
sys_getdirentry(uint32_t arg[]) {
    int fd = (int)arg[0];
    struct dirent *direntp = (struct dirent *)arg[1];
    return sysfile_getdirentry(fd, direntp, NULL);
}

static uint32_t
sys_dup(uint32_t arg[]) {
    int fd1 = (int)arg[0];
    int fd2 = (int)arg[1];
    return sysfile_dup(fd1, fd2);
}

static uint32_t
sys_pipe(uint32_t arg[]) {
    int *fd_store = (int *)arg[0];
    return sysfile_pipe(fd_store);
}

static uint32_t
sys_mkfifo(uint32_t arg[]) {
    const char *name = (const char *)arg[0];
    uint32_t open_flags = (uint32_t)arg[1];
    return sysfile_mkfifo(name, open_flags);
}

static uint32_t
sys_ioctl(uint32_t arg[])
{
  int fd = (int)arg[0];
  unsigned int cmd = arg[1];
  unsigned long data = (unsigned long)arg[2];
  return sysfile_ioctl(fd, cmd, data);
}

static uint32_t
sys_linux_mmap(uint32_t arg[])
{
  void *addr = (void*)arg[0];
  size_t len = arg[1];
  int fd = (int)arg[2];
  size_t off = (size_t)arg[3];
  return (uint32_t)sysfile_linux_mmap(addr, len, fd, off);
}

///////////////////////////////////////////

static uint32_t __sys_linux_ioctl(uint32_t args[])
{
  return 0;
}

static uint32_t
__sys_linux_dup(uint32_t arg[]) {
        int fd = (int)arg[0];
        return sysfile_dup(fd, NO_FD); 
}

static uint32_t
__sys_linux_fcntl(uint32_t arg[])
{
  return -E_INVAL;
}

static uint32_t 
__sys_linux_brk(uint32_t arg[]){
	uintptr_t brk = (uintptr_t)arg[0];
	return do_linux_brk(brk);
}

static uint32_t
__sys_linux_getdents(uint32_t arg[])
{
  int fd = (int)arg[0];
  struct linux_dirent *dir = (struct linux_dirent*)arg[1];
  uint32_t count = arg[2];
  if(count < sizeof(struct dirent))
    return -1;
  int ret = sysfile_getdirentry(fd, dir, &count);
  if(ret < 0)
    return -1;
  return count;
}


static uint32_t
__sys_linux_stat(uint32_t args[])
{
  return -1;
}

static uint32_t
__sys_linux_waitpid(uint32_t arg[]) {
	int pid = (int)arg[0];
	int *store = (int *)arg[1];
  int options = (int)arg[2];
  void *rusage = (void*)arg[3];
  if(options && rusage)
    return -E_INVAL;
	return do_linux_waitpid(pid, store);
}
#define __sys_linux_wait4 __sys_linux_waitpid

static uint32_t
__sys_linux_sched_yield(uint32_t arg[])
{
  return do_yield();
}

#define __UCORE_SYSCALL(x) [__NR_##x]  sys_##x
#define __LINUX_SYSCALL(x) [__NR_##x]  __sys_linux_##x

#define sys_dup2 sys_dup

#include <linux_unistd.h>

static uint32_t (*_linux_syscalls[])(uint32_t arg[]) = {
  __UCORE_SYSCALL(exit),
  __UCORE_SYSCALL(fork),
  __UCORE_SYSCALL(read),
  __UCORE_SYSCALL(write),
  __UCORE_SYSCALL(open),
  __UCORE_SYSCALL(close),

  __UCORE_SYSCALL(link),
  __UCORE_SYSCALL(unlink),
  __UCORE_SYSCALL(execve),
  __UCORE_SYSCALL(chdir),

  __UCORE_SYSCALL(rename),
  __UCORE_SYSCALL(mkdir),
  /* rmdir */
  __LINUX_SYSCALL(dup),
  __UCORE_SYSCALL(pipe),
  /* times */
  __LINUX_SYSCALL(brk),

  __UCORE_SYSCALL(getpid),
  __LINUX_SYSCALL(ioctl),
  __LINUX_SYSCALL(fcntl),

  __UCORE_SYSCALL(dup2),

  __LINUX_SYSCALL(stat),
  __UCORE_SYSCALL(fstat),

  __LINUX_SYSCALL(wait4),
  __UCORE_SYSCALL(fsync),
  __UCORE_SYSCALL(getcwd),

  __LINUX_SYSCALL(getdents),

  __LINUX_SYSCALL(sched_yield),

};

#define NUM_LINUX_SYSCALLS        ((sizeof(_linux_syscalls)) / (sizeof(_linux_syscalls[0])))

/* Linux EABI
 * r7 = syscall num
 * r0 - r3 = args
 * NOTE: EABI requires kernel to save lr,
 * while it is saved by user code in OABI
 */

static int __sys_linux_entry(struct trapframe *tf)
{
  unsigned int num = tf->tf_regs.reg_r[7];
  if(num < NUM_LINUX_SYSCALLS && _linux_syscalls[num]){
    uint32_t arg[4];
    arg[0] = tf->tf_regs.reg_r[0]; // arg0
    arg[1] = tf->tf_regs.reg_r[1]; // arg1
    arg[2] = tf->tf_regs.reg_r[2]; // arg2
    arg[3] = tf->tf_regs.reg_r[3]; // arg3
    tf->tf_regs.reg_r[0] = _linux_syscalls[num](arg); // calling the system call, return value in r0
    return 0;
  }

  return -1;
}

static uint32_t (*syscalls[])(uint32_t arg[]) = {
    [SYS_exit]              sys_exit,
    [SYS_fork]              sys_fork,
    [SYS_wait]              sys_wait,
    [SYS_exec]              sys_execve,
    [SYS_clone]             sys_clone,
    [SYS_exit_thread]       sys_exit_thread,
    [SYS_yield]             sys_yield,
    [SYS_kill]              sys_kill,
    [SYS_sleep]             sys_sleep,
    [SYS_gettime]           sys_gettime,
    [SYS_getpid]            sys_getpid,
    [SYS_brk]               sys_brk,
    [SYS_mmap]              sys_mmap,
    [SYS_munmap]            sys_munmap,
    [SYS_shmem]             sys_shmem,
    [SYS_putc]              sys_putc,
    [SYS_pgdir]             sys_pgdir,
    [SYS_sem_init]          sys_sem_init,
    [SYS_sem_post]          sys_sem_post,
    [SYS_sem_wait]          sys_sem_wait,
    [SYS_sem_free]          sys_sem_free,
    [SYS_sem_get_value]     sys_sem_get_value,
    [SYS_event_send]        sys_event_send,
    [SYS_event_recv]        sys_event_recv,
    [SYS_mbox_init]         sys_mbox_init,
    [SYS_mbox_send]         sys_mbox_send,
    [SYS_mbox_recv]         sys_mbox_recv,
    [SYS_mbox_free]         sys_mbox_free,
    [SYS_mbox_info]         sys_mbox_info,
    [SYS_open]              sys_open,
    [SYS_close]             sys_close,
    [SYS_read]              sys_read,
    [SYS_write]             sys_write,
    [SYS_seek]              sys_seek,
    [SYS_fstat]             sys_fstat,
    [SYS_fsync]             sys_fsync,
    [SYS_chdir]             sys_chdir,
    [SYS_getcwd]            sys_getcwd,
    [SYS_mkdir]             sys_mkdir,
    [SYS_link]              sys_link,
    [SYS_rename]            sys_rename,
    [SYS_unlink]            sys_unlink,
    [SYS_getdirentry]       sys_getdirentry,
    [SYS_dup]               sys_dup,
    [SYS_pipe]              sys_pipe,
    [SYS_mkfifo]            sys_mkfifo,
    [SYS_ioctl]             sys_ioctl,
    [SYS_linux_mmap]        sys_linux_mmap,
};



#define NUM_SYSCALLS        ((sizeof(syscalls)) / (sizeof(syscalls[0])))

void
syscall() {
    uint32_t arg[5];
    struct trapframe* tf = pls_read(current)->tf;
    int num = tf->tf_err; // SYS_xxx
    if (num == 0){
      if( __sys_linux_entry(tf) )
        goto bad_call;
      return;
    }
    if (num >= 0 && num < NUM_SYSCALLS) {
        if (syscalls[num] != NULL) {
            arg[0] = tf->tf_regs.reg_r[0]; // arg0
            arg[1] = tf->tf_regs.reg_r[1]; // arg1
            arg[2] = tf->tf_regs.reg_r[2]; // arg2
            arg[3] = tf->tf_regs.reg_r[3]; // arg3
            tf->tf_regs.reg_r[0] = syscalls[num](arg); // calling the system call, return value in r0
            return ;
        }
    }
bad_call:
    print_trapframe(tf);
    kprintf("undefined syscall %d, pid = %d, name = %s.\n",
            num, pls_read(current)->pid, pls_read(current)->name);
    do_exit(-E_KILLED);
}

