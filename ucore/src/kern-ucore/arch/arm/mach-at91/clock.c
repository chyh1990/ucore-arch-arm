#include <arm.h>
#include <clock.h>
#include <trap.h>
#include <stdio.h>
#include <kio.h>
#include <picirq.h>
#include <board.h>


volatile size_t ticks;
#define PIV_200_MS              100000  // 7.5 ms for 12MHz
/* *
 * clock_init - initialize TIMER4 to interrupt 100 times per second,
 * and then enable INT_TIMER4 (IRQ).
 * PCLK = 50 Mhz, divider = 1/16, prescaler = 24, TCNT = 1267
 * */
/* 
 * Periodic Interval Timer initialization 
 */

void
clock_init(void) {
  outw(AT91C_BASE_PIT+PIT_MR, AT91C_SYSC_PITEN | AT91C_SYSC_PITIEN | PIV_200_MS); //enable IRQ PIT
  pic_enable(AT91C_ID_SYS);
  kprintf("++ setup timer interrupts\n");
}

void clock_clear(void){
  inw(AT91C_BASE_PIT+PIT_PIVR);
}

