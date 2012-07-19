/*
 * =====================================================================================
 *
 *       Filename:  module.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/17/2012 09:02:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */
#include <module.h>
#include <kio.h>

void do_initcalls()
{
  int i;
  int cnt = __initcall_end - __initcall_start;
  kprintf("do_initcalls() %08x %08x %d calls, begin...\n", __initcall_start, __initcall_end,cnt);
  for(i=0;i < cnt; i++){
    kprintf("  calling 0x%08x\n", __initcall_start[i]);
    __initcall_start[i]();
  }
  kprintf("do_initcalls() end!\n");
}

