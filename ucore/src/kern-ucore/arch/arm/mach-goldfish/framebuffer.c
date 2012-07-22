/*
 * =====================================================================================
 *
 *       Filename:  framebuffer.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/14/2012 06:01:40 PM
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
#include <stdio.h>
#include <pmm.h>
#include <kio.h>
#include <picirq.h>
#include <framebuffer.h>
//#include "goldfish_logo.h"

/* These values *must* match the platform definitions found under
 * hardware/libhardware/include/hardware/hardware.h
 */
enum {
    HAL_PIXEL_FORMAT_RGBA_8888          = 1,
    HAL_PIXEL_FORMAT_RGBX_8888          = 2,
    HAL_PIXEL_FORMAT_RGB_888            = 3,
    HAL_PIXEL_FORMAT_RGB_565            = 4,
    HAL_PIXEL_FORMAT_BGRA_8888          = 5,
    HAL_PIXEL_FORMAT_RGBA_5551          = 6,
    HAL_PIXEL_FORMAT_RGBA_4444          = 7,
};

enum {
    FB_GET_WIDTH        = 0x00,
    FB_GET_HEIGHT       = 0x04,
    FB_INT_STATUS       = 0x08,
    FB_INT_ENABLE       = 0x0c,
    FB_SET_BASE         = 0x10,
    FB_SET_ROTATION     = 0x14,
    FB_SET_BLANK        = 0x18,
    FB_GET_PHYS_WIDTH   = 0x1c,
    FB_GET_PHYS_HEIGHT  = 0x20,
    FB_GET_FORMAT       = 0x24,

    FB_INT_VSYNC             = 1U << 0,
    FB_INT_BASE_UPDATE_DONE  = 1U << 1
};

struct goldfish_fb {
	void *reg_base;
	int irq;
	int base_update_count;
	int rotation;
  void *screen_base;
	uint32_t cmap[16];

  uint32_t fix_height;
  uint32_t fix_width;
  uint32_t var_height;
  uint32_t var_width;
  uint32_t var_bits_per_pixel;
  //RGB
  unsigned char var_coloff[4];
};

static struct goldfish_fb fb;
static int fb_ok = 0;

void
fb_init(uint32_t base, int irq) {
  if(fb_ok)
    return;
  fb.reg_base = (void*)base;
  fb.irq = irq;
	fb.fix_width = inw((uint32_t)fb.reg_base + FB_GET_WIDTH);
	fb.fix_height = inw((uint32_t)fb.reg_base + FB_GET_HEIGHT);
	fb.var_width = inw((uint32_t)fb.reg_base + FB_GET_PHYS_WIDTH);
	fb.var_height = inw((uint32_t)fb.reg_base + FB_GET_PHYS_HEIGHT);
  fb.var_bits_per_pixel = 16;

  fb.var_coloff[0] = 11;
  fb.var_coloff[1] = 5;
  fb.var_coloff[2] = 0;

  kprintf("init_goldfish_fb()\n");

  fb_ok = 1;
}

static int check_fb_drawable()
{
  return (fb_ok && fb.screen_base);
}

static void draw_test()
{
#if 0
  assert(check_fb_drawable());
  int w = fb.fix_width;
  int h = fb.fix_height;
  int x = (w - logo_width)/2;
  int y = (h - logo_height)/2;
  uint16_t *buf = (uint16_t*)fb.screen_base;
  int i,j;
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
#endif
}

int fb_alloc_buffer()
{
  assert(fb_ok);
  /* double buffer */
  uint32_t framesize = fb.fix_width * fb.fix_height * 2 * 2;
  fb.screen_base = (void*)page2kva( alloc_pages( (framesize + PGSIZE - 1) / PGSIZE) );
  memset(fb.screen_base, 0x00, framesize);
  if(fb.screen_base == NULL)
    return -1;
  outw((uint32_t)fb.reg_base + FB_SET_BASE, (uint32_t)fb.screen_base);
  kprintf("fb: allocate %dx%d\n", fb.fix_width, fb.fix_height);

  draw_test();
  return 0;
}



