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


static const char hexchars[]="0123456789abcdef";

static int hex(char ch)
{
    if ((ch >= 'a') && (ch <= 'f'))
        return (ch - 'a' + 10);
    if ((ch >= '0') && (ch <= '9'))
        return (ch - '0');
    if ((ch >= 'A') && (ch <= 'F'))
        return (ch - 'A' + 10);
    return (-1);
}


/* convert the memory pointed to by mem into hex, placing result in buf */
/* return a pointer to the last char put in buf (null) */
static char *mem2hex(char *mem, char *buf, int count)
{
    int i;
    unsigned char ch;

    for (i = 0; i < count; i++) {
        ch = *mem++;
        *buf++ = hexchars[ch >> 4];
        *buf++ = hexchars[ch % 16];
    }
    *buf = 0;
    return (buf);
}

/* convert the hex array pointed to by buf into binary to be placed in mem */
/* return a pointer to the character AFTER the last byte written */
static char *hex2mem(char *buf, char *mem, int count)
{
    int i;
    unsigned char ch;

    for (i = 0; i < count; i++) {
        ch = hex(*buf++) << 4;
        ch = ch + hex(*buf++);
        *mem++ = ch;
    }
    return (mem);
}

/*
 * WHILE WE FIND NICE HEX CHARS, BUILD AN INT
 * RETURN NUMBER OF CHARS PROCESSED
 */
int hexToInt(char **ptr, int *intValue)
{
        int numChars = 0;
        int hexValue;

        *intValue = 0;

        while (**ptr) {
                hexValue = hex(**ptr);
                if (hexValue >= 0) {
                        *intValue = (*intValue << 4) | hexValue;
                        numChars++;
                } else
                        break;

                (*ptr)++;
        }

        return (numChars);
}


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

#ifdef HAS_SDS
#include <intel_sds.h>
#define __kgdb_put_char(c) sds_putc(1, c)
#define __kgdb_get_char() sds_proc_data(1)
#define __kgdb_flush_char() while(!sds_poll_proc());
#define __kgdb_poll_char() (sds_poll_proc())
#else
#define __kgdb_put_char(c)
#define __kgdb_get_char() ('#')
#define __kgdb_flush_char()
#define __kgdb_poll_char() (0)
#endif

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
    else
      data[cnt++] = c;
    if(c == '#')
      break;
  }
  data[cnt] = 0;
  if(cnt>0 && data[cnt-1] == '#'){
    return 0;
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

static uint32_t kgdb_get_lr(struct trapframe *tf)
{
  uint32_t mode = tf->tf_sr & 0x1F;
  uint32_t cpsr = read_psrflags();
  uint32_t nc = cpsr & (~0x1F);
  uint32_t lr = 0;
  switch(mode){
    case ARM_SR_MODE_ABT:
    case ARM_SR_MODE_FIQ:
    case ARM_SR_MODE_IRQ:
    case ARM_SR_MODE_SVC:
    case ARM_SR_MODE_SYS:
    case ARM_SR_MODE_UND:
      nc |= mode;
      write_psrflags(nc);
      asm volatile ("mov %0, lr" : "=r" (lr));
      write_psrflags(cpsr);
      return lr;
    case ARM_SR_MODE_USR:
    default:
      return 0;
  }
  return 0;
}

static void kgdb_set_lr(struct trapframe *tf, uint32_t lr)
{
  uint32_t mode = tf->tf_sr & 0x1F;
  uint32_t cpsr = read_psrflags();
  uint32_t nc = cpsr & (~0x1F);
  switch(mode){
    case ARM_SR_MODE_ABT:
    case ARM_SR_MODE_FIQ:
    case ARM_SR_MODE_IRQ:
    case ARM_SR_MODE_SVC:
    case ARM_SR_MODE_SYS:
    case ARM_SR_MODE_UND:
      nc |= mode;
      write_psrflags(nc);
      asm volatile ("mov lr, %0" :: "r" (lr));
      write_psrflags(cpsr);
      break;
    case ARM_SR_MODE_USR:
    default:
      return;
  }
}

static  uint32_t gdb_regs[13+4];
static void kgdb_reg2data(struct trapframe* tf, 
  char *buf)
{
  memcpy(gdb_regs, &tf->tf_regs, sizeof(struct pushregs));
  gdb_regs[13] = tf->tf_esp;
  gdb_regs[14] = tf->tf_epc;
  gdb_regs[15] = tf->tf_epc;
  gdb_regs[16] = tf->tf_sr;
  //strcpy(buf, "1234abcdcccccccc");
  mem2hex((char*)gdb_regs, buf, sizeof(struct pushregs));
  
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
  if(kdebug_check_mem_range(start,size))
    return -1;

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
    kprintf("D>");
    rval = kgdb_get_packet(kgdb_buffer);
    if(rval){
      kprintf("kgdb_trap: fail to get command from host\n");
      goto cont_kernel;
    }
    kprintf("%s\n", kgdb_buffer);
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
        rval = kgdb_put_packet("OK");
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


