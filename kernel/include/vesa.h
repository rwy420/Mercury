#ifndef __MERCURY__VESA_H
#define __MERCURY__VESA_H


#define COLOR_RGBA(r, g, b, a)  ((uint32_t)(((r) << 24) | ((g) << 16) | ((b) << 8) | (a)))
#define COLOR_BLACK       COLOR_RGBA(0xFF, 0x00, 0x00, 0x00)
#define COLOR_WHITE       COLOR_RGBA(0xFF, 0xFF, 0xFF, 0xFF)
#define COLOR_RED         COLOR_RGBA(0xFF, 0xFF, 0x00, 0x00)
#define COLOR_GREEN       COLOR_RGBA(0xFF, 0x00, 0xFF, 0x00)
#define COLOR_BLUE        COLOR_RGBA(0xFF, 0x00, 0x00, 0xFF)
#define COLOR_YELLOW      COLOR_RGBA(0xFF, 0xFF, 0xFF, 0x00)
#define COLOR_CYAN        COLOR_RGBA(0xFF, 0x00, 0xFF, 0xFF)
#define COLOR_MAGENTA     COLOR_RGBA(0xFF, 0xFF, 0x00, 0xFF)
#define COLOR_GRAY        COLOR_RGBA(0xFF, 0x80, 0x80, 0x80)
#define COLOR_DARK_GRAY   COLOR_RGBA(0xFF, 0x40, 0x40, 0x40)
#define COLOR_LIGHT_GRAY  COLOR_RGBA(0xFF, 0xC0, 0xC0, 0xC0)
#define COLOR_ORANGE      COLOR_RGBA(0xFF, 0xFF, 0xA5, 0x00)
#define COLOR_PINK        COLOR_RGBA(0xFF, 0xFF, 0x69, 0xB4)


#include <common/types.h>
#include <memory/paging.h>

typedef struct
{
	uint32_t fb;
	uint32_t fb_width, fb_height;
} __attribute__((packed)) VesaInfoBlock;

void vesa_init();
void vesa_put_pixel(int x, int y, uint32_t color);
void vesa_putc(char c, int x, int y, uint32_t fg, uint32_t bg);
void vesa_clear();
void vesa_map(PageDirectory* pd);

#endif
