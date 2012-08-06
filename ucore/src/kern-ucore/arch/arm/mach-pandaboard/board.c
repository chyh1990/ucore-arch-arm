/*
 * =====================================================================================
 *
 *       Filename:  board.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/24/2012 01:21:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */

#include <board.h>
#include <picirq.h>
#include <serial.h>
#include <pmm.h>
#include <clock.h>

static const char* message = "Initializing Pandaboard Board...\n";

static void put_string(const char* str)
{
  while(*str)
    serial_putc(*str++);
}

void 
board_init_early(){
  put_string(message);

  pic_init_early();                 // init interrupt controller
/* use pdev bus to probe devices */
  //serial_init();
}

void
board_init()
{
  uint32_t mpu_base = (uint32_t)__ucore_ioremap(CORTEX_A9_MPU_INSTANCE_BASE , 2*PGSIZE,0);
  pic_init2(mpu_base + 0x100, mpu_base+0x1000);
	serial_init(PANDABOARD_UART0, PER_IRQ_BASE+PANDABOARD_UART3_IRQ);
}

/* no nand */
int check_nandflash(){
  return 0;
}

struct nand_chip* get_nand_chip(){
  return NULL;
}

