#include <ulib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <file.h>
#include <dir.h>
#include <unistd.h>
#include <malloc.h>
#include "goldfish_logo.h"

static void logo_loop(){
    int fd;
    int size = 800*600*2;
    unsigned short* buf = (unsigned short*)malloc(size);
    assert(buf);
    memset(buf, 0x00, size);

    int i,j;
    int w = 800;
    int h = 600;
    int x = (w - logo_width)/2;
    int y = (h - logo_height)/2;
    int v = 2, u = 2;

    while(1){
      x += u;
      y += v;
      if(x<0){
        x = 0;
        u = -u;
      }
      else if(x+logo_width>=w){
        x = w - logo_width -1;
        u = -u;
      }
      if(y<0){
        y = 0;
        v = -v;
      }
      else if(y+logo_height>=h){
        y = h - logo_height - 1;
        v = -v;
      }

      fd = open("fb0:", O_RDWR);
      //cprintf("fd = %d\n", fd);
      if(fd<0)
        return ;
      unsigned char pixel[4];
      uint16_t color;
      char * data = logo_header_data;
      for(i=y*w;i<w*(y+logo_height);i+=w){
        for(j=x;j<x+logo_width;j++){
          HEADER_PIXEL(data, pixel);
          color = ((pixel[0]>>3)<<11)
            |((pixel[1]>>2)<<5)
            |((pixel[2]>>3));
          buf[i + j] = color;
        }
      }
      write(fd, buf, size);
      close(fd);
    }

    free(buf);

}

int
main(int argc, char *argv[]) {
  int i;
  cprintf("Hello world!!.\n");
  cprintf("I am process %d.\n", getpid());
  cprintf("ARG: %d:\n", argc);
  for(i=0;i<argc;i++)
    cprintf("ARG %d: %s\n", i, argv[i]);

  int fd = -1;
#if 0
  int test = 1234567;
  fd = open("hzfchar:", O_RDWR);
  cprintf("fd = %d\n", fd);
  test = write(fd, &test, sizeof(int));
  cprintf("ret = %d\n", test);
  i = read(fd, &test, sizeof(int));
  cprintf("ret = %d, %d\n",i, test);
  i = read(fd, &test, sizeof(int));
  cprintf("ret = %d, %d\n",i, test);
#endif
  logo_loop();

  cprintf("hello pass.\n");
  return 0;
}

