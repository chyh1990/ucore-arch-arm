/*
 * =====================================================================================
 *
 *       Filename:  board.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/17/2012 02:19:11 PM
 *       Revision:  none
 *       Compiler:  arm-linux-gcc 4.4
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */


#ifndef  MACH_BOARD_PANDABOARD_H
#define  MACH_BOARD_PANDABOARD_H

#define PANDABOARD_IO_START 0xFF000000
#define PANDABOARD_IO_FREE_START 0xff010000
#define PANDABOARD_UART0 (PANDABOARD_IO_FREE_START+0x2000) 


#define PANDABOARD_TIMER0_1_BASE (PANDABOARD_IO_START+0x3000)

#define PANDABOARD_VIC_BASE      (PANDABOARD_IO_START+0x00)

#ifndef __io_address
#define __io_address(x) (x)
#endif

//IRQ 
#define TIMER0_IRQ 3

#define UART_RXIM (1<<4)

//extern macro

#define SDRAM0_START UCONFIG_DRAM_START
#define SDRAM0_SIZE  UCONFIG_DRAM_SIZE //256M

#define IO_SPACE_START PANDABOARD_IO_START
#define IO_SPACE_SIZE  0x1000000

//#define HAS_RAMDISK
//#define HAS_FRAMEBUFFER
//#define HAS_SHARED_KERNMEM
//#define KERNEL_PHY_BASE 0x100000

#ifndef __ASSEMBLY__

#define UART0_TX 		((volatile unsigned char*) PANDABOARD_UART0 + 0x00)
//#define INITIAL_LOAD    ((volatile uintptr_t *) (0x1000))

extern void board_init(void);

#endif

#endif  
