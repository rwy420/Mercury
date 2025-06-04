#ifndef __MERCURY__VESA_H
#define __MERCURY__VESA_H

#define RGB565_BLACK	0x0000
#define RGB565_WHITE	0xFFFF
#define RGB565_RED		0xF800
#define RGB565_GREEN	0x07E0
#define RGB565_BLUE		0x001F
#define RGB565_YELLOW	0xFFE0
#define RGB565_CYAN		0x07FF
#define RGB565_MAGENTA	0xF81F
#define RGB565_GRAY		0x8410
#define RGB565_ORANGE	0xFDA0

#include <common/types.h>
#include <memory/paging.h>

typedef struct
{
	uint32_t fb;
	uint32_t fb_width, fb_height;
} __attribute__((packed)) VesaInfoBlock;

void vesa_init();
void vesa_put_pixel(int x, int y, uint16_t color);
void vesa_putc(char c, int x, int y, uint16_t fg, uint16_t bg);
void vesa_clear();
void vesa_map(PageDirectory* pd);

#endif
