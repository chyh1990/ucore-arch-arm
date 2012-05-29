/*
 * =====================================================================================
 *
 *       Filename:  kshm_test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/29/2012 10:35:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */
#include <ulib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <file.h>
#include <dir.h>
#include <unistd.h>

#define printf(...)                 fprintf(1, __VA_ARGS__)
#define KSHM_BASE 0xD0008000
#define KSHM_SIZE (1<<20) /* 1M */

static unsigned char buf[512];

int main(int argc, char* argv[])
{
  void *kshm_base = (void*)KSHM_BASE;
  uint32_t a = *(uint32_t*)kshm_base;
  assert(kshm_base!=NULL);
  printf("[0]: %08x\n", a);
  int fd = open("sdsfile.bin", O_WRONLY|O_TRUNC|O_CREAT);
  if(fd < 0){
    printf("failed to open file.\n");
    return fd;
  }
  /* KSHM_BASE is in kernel space, copy to user space first */
  memcpy(buf, kshm_base, 512);
  int len = write(fd, buf, 512);
  if(len < 0)
    return len;
  close(fd);
  return 0;
}

