/*
 *  bootmain.c
 *  copy kernel image to ram
 *  kernel should be placed in first sector of SROM, which is INITIAL_LOAD
 *
 *  author: YU KuanLong / HE Alexis
 *  part of the code is written by WANG JianFei
 *  date:   2011-3-21
 */

#include "elf.h"

#define ELFHDR		((struct elfhdr *) (BOOTLOADER_BASE+0x1000))      // scratch space

#define UART_BASE 0x48020000
#define UART_TX (UART_BASE+0x00)
#define UART_LSR (UART_BASE+0x14)

#define TX_SR_E 0x20

static inline void outw(uint32_t addr, uint32_t v)
{
  *(volatile uint32_t*)addr = v;
}

static inline uint32_t inw(uint32_t addr)
{
  return *(volatile uint32_t*)addr;
}


/* uart_putc, uart_put_s
 * write on UART0 */
static void uart_putc(int c)
{
  while((inw(UART_LSR) & TX_SR_E) == 0);
  if(c=='\n')
    outw(UART_TX, '\r');
  outw(UART_TX, c & 0xff);
}

static void uart_puts(const char *p)
{
  while (*p)
    uart_putc(*p++);
}


/* *
 * readseg - read @count bytes at @offset from kernel into virtual address @va,
 * might copy more than asked.
 * */
static void
readseg(uintptr_t va, uint32_t count, uint32_t offset) {
  __memcpy((void*)va, (void*)((uint8_t*)ELFHDR + offset), count);
}

/* bootmain - the entry of bootloader */
void
bootmain(void) {
  extern char __bss_start__[], __bss_end__[];
  char *ptr = __bss_start__;
  const char *booterror = "Unknown";
  for(;ptr<__bss_end__;ptr++)
    *ptr = 0;

  uart_puts("Booting, please wait...\n");

  // is this a valid ELF?
  if (ELFHDR->e_magic != ELF_MAGIC) {
    booterror = "Bad ELF Magic";
    goto bad;
  }

  struct proghdr *ph, *eph;

  // load each program segment (ignores ph flags)
  ph = (struct proghdr *)((uintptr_t)ELFHDR + ELFHDR->e_phoff);
  eph = ph + ELFHDR->e_phnum;
  for (; ph < eph; ph ++) {
    readseg(ph->p_va, ph->p_memsz, ph->p_offset);
  }

  // call the entry point from the ELF header
  // note: does not return
  uart_puts("Jump to kernel\n");
  ((void (*) (void))(ELFHDR->e_entry))();

bad:
  uart_puts("Error: ");
  uart_puts(booterror);
  uart_puts("\n");

  /* do nothing */
  while (1);
}

