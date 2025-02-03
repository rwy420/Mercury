#include <drivers/vga.h>
#include <qslibc/qs_io.h>
#include <qslibc/qs_mem.h>

#define VGA_WIDTH 640
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

unsigned char g_640x480x16[61] =
{
/* MISC */
	0xE3,
/* SEQ */
	0x03, 0x01, 0x08, 0x00, 0x06,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E,
	0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xEA, 0x0C, 0xDF, 0x28, 0x00, 0xE7, 0x04, 0xE3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0F,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x01, 0x00, 0x0F, 0x00, 0x00
};

volatile unsigned char* vga_memory = (unsigned char*)0xA0000;

void vga_write_registers()
{
	unsigned char* registers = g_640x480x16;

	outb(MISC_PORT, *(registers++));

	for(unsigned char i = 0; i < 5; i++)
	{
		outb(SEQUENCER_INDEX_PORT, i);
		outb(SEQUENCER_DATA_PORT, *(registers++));
	}

	outb(CRTC_INDEX_PORT, 0x03);
	outb(CRTC_DATA_PORT, inb(CRTC_DATA_PORT) | 0x80);
	outb(CRTC_INDEX_PORT, 0x11);
	outb(CRTC_DATA_PORT, inb(CRTC_DATA_PORT) & ~0x80);

	registers[0x03] = registers[0x03] | 0x80;
	registers[0x11] = registers[0x11] & ~0x80;

	for(unsigned char i = 0; i < 25; i++)
	{
		outb(CRTC_INDEX_PORT, i);
		outb(CRTC_DATA_PORT, *(registers++));
	}

	for(unsigned char i = 0; i < 9; i++)
	{
		outb(CONTROLLER_INDEX_PORT, i);
		outb(CONTROLLER_DATA_PORT, *(registers++));
	}

	for(unsigned char i = 0; i < 21; i++)
	{
		inb(ATTRIBUTE_CONTROLLER_RESET_PORT);
		outb(ATTRIBUTE_CONTROLLER_INDEX_PORT, i);
		outb(ATTRIBUTE_CONTROLLER_WRITE_PORT, *(registers++));
	}

	inb(ATTRIBUTE_CONTROLLER_RESET_PORT);
	outb(ATTRIBUTE_CONTROLLER_INDEX_PORT, 0x20);
}

void vga_set_plane(unsigned char plane)
{
    outb(0x3C4, 0x02); // Select Map Mask register
    outb(0x3C5, 1 << plane); // Enable only the specified plane
}

void vga_set_pixel(int x, int y, unsigned char color)
{
	int byte_offset = (y * (VGA_WIDTH / 8)) + (x / 8);
	unsigned char bit_mask = 1 << (7 - (x % 8));

	for(int plane = 0; plane < 4; plane++)
	{
		vga_set_plane(plane);

		unsigned char pixel_data = vga_memory[byte_offset];
		if(color & (1 << plane))
			pixel_data |= bit_mask;
		else 
			pixel_data &= ~bit_mask;

		vga_memory[byte_offset] = pixel_data;
	}
}

void vga_fill(unsigned char color) {
	for (int plane = 0; plane < 4; plane++) {
        vga_set_plane(plane);
        unsigned char fill_value = (color & (1 << plane)) ? 0xFF : 0x00;

        // Use memset to fill the plane memory efficiently
        memset((void*)vga_memory, fill_value, VGA_WIDTH * VGA_HEIGHT / 8);
    }
}

void vga_init()
{
	vga_write_registers();
	while(1)
	{
		for(char c = 1; c < 10; c++)
		{
			vga_fill(c);
		}
	}
}
