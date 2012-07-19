/*
 * =====================================================================================
 *
 *       Filename:  linux-glue.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/17/2012 08:56:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */

#ifndef _LINUX_GLUE_H
#define _LINUX_GLUE_H

#include <kio.h>

#define printk(...) kprintf(__VA_ARGS__)

#endif

