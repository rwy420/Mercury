#include <vm.h>
#include <vesa.h>
#include <psf1.h>
#include <memory/common.h>
#include <memory/paging.h>
#include <printf.h>

extern VesaInfo g_vesa_info;
uint32_t* vesa_fb;
static uint16_t x = 0, y = 0;

int syscall_printf(VFSNode* node, void* buffer, uint32_t offset, size_t length)
{
	kprintf((string) buffer);
	return 0;
}

void vesa_init()
{
	vesa_fb = (uint32_t*) g_vesa_info.fb;
}

void vesa_put_pixel(int x, int y, uint32_t color)
{
	vesa_fb[y * g_vesa_info.fb_width + y] = color;
}

void vesa_putc(char c, int x, int y, uint32_t fg, uint32_t bg)
{
	uint8_t *glyph = ((uint8_t*)(&u_vga16_psf) + sizeof(PSF1Header)) + (c * 0xE);
	
	for (int row = 0; row < 0xE; row++)
	{
        uint8_t bits = glyph[row];

        for (int col = 0; col < 8; col++)
		{
            int pixel_x = x + col;
            int pixel_y = y + row;
            int index = pixel_y * g_vesa_info.fb_width + pixel_x;

            if (bits & (1 << (7 - col))) vesa_fb[index] = fg;
            else vesa_fb[index] = bg;
        }
    }
}

void vesa_putc_inc(char c)
{
	dbg_putc(c);
	switch(c)
	{
		case '\n':
			y += 0xE;
			x = 0;
			break;

		case ' ':
			x += 0x8;
			break;

		default:
			vesa_putc(c, x, y, COLOR_WHITE, COLOR_BLACK);
			x += 0x8;
	}

	if(x >= g_vesa_info.fb_width - 0x10)
	{
		y += 0xE;
		x = 0;
	}

	if(y >= g_vesa_info.fb_height - 0x20)
	{
		vesa_clear();
	}
}

void vesa_clear()
{
	x = 0;
	y = 0;
	memset(vesa_fb, 0x0, g_vesa_info.fb_height * g_vesa_info.fb_width * 4);
}

void vesa_map(PageDirectory* pd)
{
	for(int i = 0; i  < g_vesa_info.fb_height * g_vesa_info.fb_width * 4; i += PAGE_SIZE)
	{
		void* address = (void*) (g_vesa_info.fb + i);
		map_page_pd(pd, address, address);
	}
}
