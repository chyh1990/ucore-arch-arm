/*
 * =====================================================================================
 *
 *       Filename:  clock.c
 *
 *    Description:  SP804 support, versatilepb
 *
 *        Version:  1.0
 *        Created:  03/25/2012 08:39:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */

#include <arm.h>
#include <board.h>
#include <clock.h>
#include <trap.h>
#include <stdio.h>
#include <kio.h>
#include <picirq.h>

#define TIMER0_INTERVAL  10000000 //10ms

volatile size_t ticks = 0;

static void reload_timer()
{
}

void clock_clear(void){
}

volatile uint64_t jiffies_64;
unsigned long volatile jiffies;

static int clock_int_handler(int irq, void * data)
{
  ticks++;
  jiffies ++;
  jiffies_64++;
  //if(ticks % 100 == 0)
  //  serial_putc('A');
  extern void run_timer_list();
  run_timer_list();
  reload_timer(); 
  clock_clear();
  return 0;
}

void
clock_init(void) {
  //TODO
  kprintf("TODO clock_init()\n");
  register_irq(TIMER0_IRQ, clock_int_handler, 0);
  pic_enable(TIMER0_IRQ);
}


