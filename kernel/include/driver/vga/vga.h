#ifndef __QS__DRIVER__VGA__VGA_H
#define __QS__DRIVER__VGA__VGA_H

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

#include <core/types.h>

void vga_write_registers(uint8_t* registers);
uint8_t* get_frame_buffer_segment();
bool vga_supports_mode(uint32_t width, uint32_t height, uint32_t color_depth);
bool vga_set_mode(uint32_t width, uint32_t height, uint32_t color_depth);
void vga_put_pixel(uint32_t x, uint32_t y, uint8_t color);
void vga_bluescreen();

#endif
