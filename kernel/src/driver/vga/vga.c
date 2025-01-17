#include <driver/vga/vga.h>
#include <hardware/port.h>

uint8_t* frame_buffer_segment;

uint8_t g_320x200x256[61] =
	{
		/* MISC */
		0x63,
		/* SEQ */
		0x03, 0x01, 0x0F, 0x00, 0x0E,
		/* CRTC */
		0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
		0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
		0xFF,
		/* GC */
		0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
		0xFF,
		/* AC */
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
		0x41, 0x00, 0x0F, 0x00,	0x00
};

void vga_write_registers(uint8_t* registers)
{
	outb(MISC_PORT, *(registers++));

	for(uint8_t i = 0; i < 5; i++)
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

	for(uint8_t i = 0; i < 25; i++)
	{
		outb(CRTC_INDEX_PORT, i);
		outb(CRTC_DATA_PORT, *(registers++));
	}

	for(uint8_t i = 0; i < 9; i++)
	{
		outb(CONTROLLER_INDEX_PORT, i);
		outb(CONTROLLER_DATA_PORT, *(registers++));
	}

	for(uint8_t i = 0; i < 21; i++)
	{
		inb(ATTRIBUTE_CONTROLLER_RESET_PORT);
		outb(ATTRIBUTE_CONTROLLER_INDEX_PORT, i);
		outb(ATTRIBUTE_CONTROLLER_WRITE_PORT, *(registers++));
	}

	inb(ATTRIBUTE_CONTROLLER_RESET_PORT);
	outb(ATTRIBUTE_CONTROLLER_INDEX_PORT, 0x20);
}

uint8_t* vga_get_frame_buffer_segment()
{
	outb(CONTROLLER_INDEX_PORT, 0x06);
	uint8_t segment_number = ((inb(CONTROLLER_DATA_PORT) >> 2) & 0x03);

	switch(segment_number)
	{
		default:
		case 0: return (uint8_t*) 0x00000;
		case 1: return (uint8_t*) 0xA0000;
		case 2: return (uint8_t*) 0xB0000;
		case 3: return (uint8_t*) 0xB8000;
	}
}

bool vga_supports_mode(uint32_t width, uint32_t height, uint32_t color_depth)
{
	return width == 320 && height == 200 && color_depth == 8;
}

bool vga_set_mode(uint32_t width, uint32_t height, uint32_t color_depth)
{
	if(!vga_supports_mode(width, height, color_depth)) return false;
	
	vga_write_registers(g_320x200x256);
	frame_buffer_segment = vga_get_frame_buffer_segment();

	return true;
}

void vga_put_pixel(uint32_t x, uint32_t y, uint8_t color)
{
	uint8_t* pixel_address = frame_buffer_segment + 320 * y + x;
	*pixel_address = color;
}

void vga_bluescreen()
{
	for(int y = 0; y < 200; y++)
		for(int x = 0; x < 320; x++)
			vga_put_pixel(x, y, 0x01);
}
