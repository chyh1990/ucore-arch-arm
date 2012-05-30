#include <board.h>
#include <types.h>
#include <stdio.h>
#include <kio.h>
#include <kdebug.h>
#include <string.h>
#include <console.h>
#include <picirq.h>
#include <trap.h>
#include <clock.h>
#include <intr.h>
#include <pmm.h>
#include <vmm.h>
#include <ide.h>
#include <swap.h>
#include <proc.h>
#include <assert.h>
#include <atomic.h>
#include <mp.h>
#include <sched.h>
#include <fs.h>
#include <sync.h>
#include <ramdisk.h>


#include <yaffs2_direct/yaffsfs.h>

#ifdef printf
#undef printf
#endif



#define _PROBE_() kprintf("PROBE %s: %d\n", __FILE__, __LINE__);

// Very important:
// At the boot, exceptions handlers have not been defined
// Copy the address of exceptions handlers (defined in vectors.S) to 0x24 
// (or Ox20 if we decide to change reset handler)
// XXX it works only when 0x0 is a sram/sdram
extern char __vector_table, __vector_table_end;
static inline void
exception_vector_init(void) {
  	memcpy( (void*)0x0, (void*)&__vector_table,
		&__vector_table_end - &__vector_table);
}

int kern_init(void) __attribute__((noreturn));


static void test_yaffs()
{
//  mtd_erase_partition(get_nand_chip(),"data");
  yaffs_mount("/data");
  kprintf("DUMP\n");
  yaffs_dump_dir("/data");
  yaffs_mkdir("/data/td", S_IREAD|S_IWRITE);
  yaffs_mkdir("/data/td/d1", S_IWRITE|S_IREAD);
  //int fd = yaffs_unlink("/data/test.txt");
  //kprintf("@@@@@@@ fd %d %s\n", fd, yaffs_error_to_str(yaffs_get_error())); 
  int fd = yaffs_open("/data/test.txt",O_RDONLY, 0);
  //int fd = yaffs_open("/data/td/test.txt",O_RDWR|O_CREAT, S_IREAD|S_IWRITE);
  kprintf("@@@@ fd %d %s\n", fd, yaffs_error_to_str(yaffs_get_error())); 
  char buf[32];
  yaffs_write(fd, "abc",3);
  int ret = yaffs_read(fd, buf, 5);
  buf[ret] = 0;
  buf[16] = 0;
  kprintf("@@@@ read: %d  %s\n", ret, buf);
  yaffs_close(fd);
  yaffs_sync("/data");
  kprintf("DUMP\n");
  yaffs_dump_dir("/data");
  yaffs_unmount("/data");


}

void __test_bp()
{
  __asm__ volatile(".word 0xe7fddefe");
}

void check_bp()
{
  kprintf("check bp...\n");
  __test_bp();
  kprintf("check bp done...\n");
}

int
kern_init(void) {
  extern char edata[], end[];
  memset(edata, 0, end - edata);

  exception_vector_init();
#ifdef HAS_RAMDISK
  check_initrd();
#endif

  board_init();

  cons_init();                // init the console

  const char *message = "(THU.CST) os is loading ...";
  kprintf("%s\n\n", message);

	/* Only to initialize lcpu_count. */
	mp_init ();

  print_kerninfo();

  pmm_init();                 // init physical memory management
  pmm_init_ap();

  kprintf("pmm_init() done.\n");

  vmm_init();                 // init virtual memory management
  _PROBE_();

  clock_init();               // linux put tick_init in kernel_main, so do we~

  //intr_enable();

  sched_init();               // init scheduler
  _PROBE_();
  proc_init();                // init process table
  _PROBE_();
    sync_init();                // init sync struct

  ide_init();                 // init ide devices
  _PROBE_();
  //swap_init();                // init swap
  _PROBE_();
  fs_init();                  // init fs
  _PROBE_();

#ifdef HAS_NANDFLASH
  yaffs_start_up();
  //test_yaffs();
  yaffs_vfs_init();
#elif defined HAS_SIM_YAFFS 
  kprintf("init: using yramsim\n");
  //emulated yaffs start up
  yaffsfs_OSInitialisation();
  yramsim_CreateRamSim("data", 1,20, 2, 16);
  test_yaffs();

  yaffs_vfs_init();

#endif

  check_bp();

  intr_enable();              // enable irq interrupt

  cpu_idle();                 // run idle process
}

