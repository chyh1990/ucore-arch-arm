/*
 * =====================================================================================
 *
 *       Filename:  kgdb-stub.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/31/2012 12:42:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */
#ifndef __DEBUG_KGDB_STUB_H
#define __DEBUG_KGDB_STUB_H

#define KGDB_BP_INSTR 0xe7fddefe 

void kgdb_init();
int setup_bp(uint32_t addr);
int remove_bp(uint32_t addr);
int kgdb_trap(struct trapframe* tf);

static inline void kgdb_breakpoint()
{
  __asm__ volatile(".word 0xe7fddefe");
}

#endif
