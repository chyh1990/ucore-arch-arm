/*
 * =====================================================================================
 *
 *       Filename:  kgdb-stub.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/31/2012 12:43:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */

#include <board.h>
#include <memlayout.h>
#include <types.h>
#include <arm.h>
#include <stdio.h>
#include <trap.h>
#include <kio.h>
#include <kdebug.h>
#include <string.h>
#include <kgdb-stub.h>

#define MAX_KGDB_BP      16

#define SBPT_AVAIL (1<<0)

struct soft_bpt{
    uint32_t    addr;       /* address breapoint is present at */
    uint32_t    inst;       /* replaced instruction */
    uint32_t    flags;
}; // __attribute__((packed));

struct soft_bpt breakpoints[MAX_KGDB_BP] = {{0}};

static struct soft_bpt* find_bp(uint32_t addr)
{
  int i = 0;
  for(;i<MAX_KGDB_BP;i++){
    if(breakpoints[i].addr == addr 
      && (breakpoints[i].flags & SBPT_AVAIL))
      return &breakpoints[i];
  }
  return NULL;
}
static struct soft_bpt* find_free_bp()
{
  int i;
  for(i = 0;i < MAX_KGDB_BP; i++){
    if (!(breakpoints[i].flags & SBPT_AVAIL))
      return &breakpoints[i];
  }
  return NULL;
}

int remove_bp(uint32_t addr)
{
  struct soft_bpt *bp = find_bp(addr);
  if(!bp)
    return -1;
  /* restore */
  *(uint32_t*)(bp->addr) = bp->inst;
  bp->flags = 0;
  return 0;
}

static struct soft_bpt *install_bp(uint32_t addr){
  struct soft_bpt *bp = find_free_bp();
  if(find_bp(addr))
    return NULL;
  if(!bp)
    return NULL;
  bp->addr = addr;
  bp->inst = *(uint32_t*)addr;
  if(bp->inst == KGDB_BP_INSTR)
    return NULL;
  /* patch */
  *(uint32_t*)addr = KGDB_BP_INSTR;
  bp->flags = SBPT_AVAIL;
  return bp;
}

int setup_bp(uint32_t addr)
{
  return (install_bp(addr)==NULL)?-1:0;
}

void kgdb_init()
{
  int i;
  for(i=0;i<MAX_KGDB_BP;i++)
    breakpoints[i].flags = 0;

}

int kgdb_trap(struct trapframe* tf)
{
  uint32_t pc = tf->tf_epc - 4;
  //uint32_t inst = *(uint32_t *)(pc);
  struct soft_bpt *bp = find_bp(pc);
  int compilation_bp = (bp==NULL);
  start_debug();
  kprintf("BP hit: compile=%d, pc=0x%08x...\n",
      compilation_bp, pc);
  end_debug();
  /* soft bp */
  if(!compilation_bp){
    /* one shot bp */
    remove_bp(pc);
    tf->tf_epc = pc;
  }
  return 0;
}


