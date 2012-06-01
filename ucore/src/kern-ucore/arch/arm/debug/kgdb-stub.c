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
#include <assert.h>
#include <kgdb-stub.h>

#define MAX_KGDB_BP      16

#define SBPT_AVAIL (1<<0)
#define SBPT_ACTIVE (1<<1)

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

static void activate_bps()
{
  int i = 0;
  for(;i<MAX_KGDB_BP;i++){
    if((breakpoints[i].flags & SBPT_AVAIL)
      && !(breakpoints[i].flags & SBPT_ACTIVE)
      ){
        breakpoints[i].inst = *(uint32_t*)breakpoints[i].addr;
        breakpoints[i].flags |= SBPT_ACTIVE;
        *(uint32_t*)breakpoints[i].addr = KGDB_BP_INSTR;
    }
  }

}

static void deactivate_bps()
{
  int i = 0;
  for(;i<MAX_KGDB_BP;i++){
    if((breakpoints[i].flags & SBPT_AVAIL)
      && (breakpoints[i].flags & SBPT_ACTIVE)
      ){
        *(uint32_t*)breakpoints[i].addr = breakpoints[i].inst;
        breakpoints[i].flags &= ~SBPT_ACTIVE;
    }
  }

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
  if(bp->inst == KGDB_BP_INSTR)
    return NULL;
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


static char kgdb_buffer[512];

#define __kgdb_put_char(c)
#define __kgdb_get_char() (-1)
#define __kgdb_flush_char()
#define __kgdb_poll_char() (0)

static int kgdb_get_packet(char *data)
{
  int cnt = 0;
  int c;
  while(1){
    c = __kgdb_get_char();
    if(c==-1)
      __kgdb_poll_char();
    else if(c=='$')
      break;
  }
  data[cnt++] = '$'; 
  while(cnt < 500){
    c = __kgdb_get_char();
    if(c==-1)
      __kgdb_poll_char();
    else if(c=='#')
      break;
    else
      data[cnt++] = c;
  }
  data[cnt] = 0;
  if(cnt>0 && data[cnt-1] == '#'){
    return cnt - 1;
  }else{
    return -1;
  }
}

/* add $# automatically */
static int kgdb_put_packet(char *data)
{
  int len = strlen(data);
  int i;
  __kgdb_put_char('$');
  for(i = 0;i<len;i++)
    __kgdb_put_char(data[i]);
  __kgdb_put_char('#');
  __kgdb_flush_char();
  return 0;
}

static void kgdb_reg2data(struct trapframe* tf, 
  char *buf)
{
  strcpy(buf, "1234abcdcccccccc");
}

#define _IS_SEP(x) ( (x) == ',' || (x)=='#' || (x)==':')
static int kgdb_parse(char *buf, uint32_t arg[], int maxlen)
{
  char *ptr = buf;
  int cnt = 0;
  while(*ptr){
    if(_IS_SEP(*ptr)){
      *ptr = 0;
      arg[cnt++] = kgdb_atoi16(buf);
      buf = ptr + 1; 
      if(cnt >= maxlen)
        return cnt;
    }
    ptr++;
  }
  return cnt; 
}

static int kgdb_sendmem(uint32_t start, uint32_t size)
{
  return 0;
}

static int trap_reentry = 0;
int kgdb_trap(struct trapframe* tf)
{
  int rval = -1;
  uint32_t arg[5];
  if(trap_reentry)
    panic("kgdb_trap reentry\n");
  trap_reentry = 1;
  uint32_t pc = tf->tf_epc - 4;
  //uint32_t inst = *(uint32_t *)(pc);
  struct soft_bpt *bp = find_bp(pc);
  int compilation_bp = (bp==NULL);

  deactivate_bps();

  start_debug();

  kprintf("BP hit: compile=%d, pc=0x%08x...\n",
      compilation_bp, pc);
#if 0
  rval = kgdb_parse("6a1bbb,2#", arg, 5);
  kprintf("## %d %08x %08x %08x\n", rval, arg[0], arg[1], arg[2]);
  rval = kgdb_parse("6a1bbb,29:a340#", arg, 5);
  kprintf("## %d %08x %08x %08x\n", rval, arg[0], arg[1], arg[2]);
#endif

  /* tell host we are ready */
  rval = kgdb_put_packet("BP");

  while(1){
    rval = kgdb_get_packet(kgdb_buffer);
    if(rval){
      kprintf("kgdb_trap: fail to get command from host\n");
      goto cont_kernel;
    }
    kprintf("kgdb_trap: host: %s\n", kgdb_buffer);
    switch(kgdb_buffer[1]){
      /* reg */
      case 'g':
        kgdb_reg2data(tf, kgdb_buffer);
        rval = kgdb_put_packet(kgdb_buffer);
        break;
      case 'G':
        break;
      /* clear bp */
      case 'z':
      /* add bp */
      case 'Z':
        rval = kgdb_parse(kgdb_buffer+3, arg, 5);
        if(rval!=2){
          kprintf("kgdb_trap: invalid param\n");
          break;
        }
        if(kgdb_buffer[1] == 'z'){
          remove_bp(arg[0]);
        }else{
          rval = setup_bp(arg[0]);
          if(rval){
            kgdb_put_packet("E10");
          }
        }
        kgdb_put_packet("OK");
        break;
      /* read mem */
      case 'm':
        rval = kgdb_parse(kgdb_buffer+3, arg, 5);
        if(rval!=2){
          kprintf("kgdb_trap: invalid param\n");
          break;
        }
        if(kgdb_sendmem(arg[0], arg[1])){
          kgdb_put_packet("E13");
        }else{
          kgdb_put_packet("OK");
        }
        break;
      /* modify mem */
      case 'M':
        kgdb_put_packet("E11");
        break;
      /* continue */
      case 'D':
      case 'c':
      case 's':
        kgdb_put_packet("OK");
        goto cont_kernel;
      default:
        kprintf("kgdb_trap: invalid command '%c'\n", kgdb_buffer[1]);
    }
  }

cont_kernel:
  end_debug();
  /* soft bp */
  if(!compilation_bp){
    /* one shot bp */
    remove_bp(pc);
    tf->tf_epc = pc;
  }
  activate_bps();
  trap_reentry = 0;
  return 0;
}


