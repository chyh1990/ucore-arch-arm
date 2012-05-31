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
#include <stdio.h>
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

static int remove_bp(uint32_t addr)
{
  struct soft_bpt *bp = find_bp(addr);
  if(!bp)
    return -1;
  /* restore */
  *(uint32_t*)(bp->addr) = bp->inst;
  return 0;

}

void kgdb_init()
{

}

