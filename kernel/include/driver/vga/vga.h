#ifndef __MERCURY__DRIVER__VGA__VGA_H
#define __MERCURY__DRIVER__VGA__VGA_H

#include <common/types.h>

#define VGA_WIDTH 720
#define VGA_HEIGHT 480

#define MISC_PORT 0x3C2
#define CRTC_INDEX_PORT 0x3D4
#define CRTC_DATA_PORT 0x3D5
#define SEQUENCER_INDEX_PORT 0x3C4
#define SEQUENCER_DATA_PORT 0x3C5
#define CONTROLLER_INDEX_PORT 0x3CE
#define CONTROLLER_DATA_PORT 0x3CF
#define ATTRIBUTE_CONTROLLER_INDEX_PORT 0x3C0
#define ATTRIBUTE_CONTROLLER_READ_PORT 0x3C1
#define ATTRIBUTE_CONTROLLER_WRITE_PORT 0x3C0
#define ATTRIBUTE_CONTROLLER_RESET_PORT 0x3DA

void vga_write_registers();
void vga_set_plane(uint8_t plane);
void vga_set_pixel(int x, int y, uint8_t color);
void draw_rectangle(int x, int y, int w, int h, uint8_t color);
void vga_fill(uint8_t color);
void vga_init();

#endif
