#ifndef __MERCURY__VESA_H
#define __MERCURY__VESA_H

#include <common/types.h>

typedef struct
{
	uint32_t fb;
	uint16_t fb_width, fb_height;
} VesaInfoBlock;

void vesa_init();
void vesa_put_pixel(int x, int y, uint16_t color);
void vesa_putc(char c, int x, int y, uint16_t fg, uint16_t bg);
void vesa_clear();

#endif
