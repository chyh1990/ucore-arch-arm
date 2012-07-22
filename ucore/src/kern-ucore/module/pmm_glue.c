/*
 * =====================================================================================
 *
 *       Filename:  pmm_glue.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/21/2012 03:37:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */
#include <pmm.h>
#include <pmm_glue.h>


extern void* ucore_kva_alloc_pages(size_t n)
{
  return page2kva(alloc_pages(n));
}

