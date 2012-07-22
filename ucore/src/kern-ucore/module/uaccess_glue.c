/*
 * =====================================================================================
 *
 *       Filename:  uaccess_glue.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/21/2012 10:14:39 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */

#include <vmm.h>
#include <proc.h>

unsigned long __ucore_copy_to_user(void *to, const void *from, unsigned long n)
{
  int ret = 0;
  struct mm_struct *mm = pls_read(current)->mm;
  lock_mm(mm);
  ret = copy_to_user(mm, to, from, n);
  unlock_mm(mm);
  if(ret)
    return 0;
  return n;
}


unsigned long __ucore_copy_from_user(void *to, const void *from, unsigned long n)
{
  int ret = 0;
  struct mm_struct *mm = pls_read(current)->mm;
  lock_mm(mm);
  ret = copy_from_user(mm, to, from, n, 0);
  unlock_mm(mm);
  if(ret)
    return 0;
  return n;
}
