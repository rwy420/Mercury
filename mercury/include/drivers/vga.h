#ifndef __MERCURY__DRIVERS__VGA_H
#define __MERCURY__DRIVERS__VGA_H

void vga_write_registers();
void vga_set_plane(unsigned char plane);
void vga_set_color(int index, unsigned int red, unsigned int green, unsigned int blue);
void vga_set_pixel(int x, int y, unsigned char color);
void vga_fill(unsigned char color);
void vga_init();

#endif
