#include "./screen.h"
#include "./vesa.h"

static uint16_t x = 0, y = 0;
extern uint16_t* vesa_fb;

void printf(string str)
{
	for(int i = 0; str[i] != '\0'; i++)
	{
		switch(str[i])
		{
			case '\n':
				y += 0xE;
				x = 0;
				break;

			case ' ':
				x += 0x8;
				break;

			default:
				vesa_putc(str[i], x, y, 0xFFFF, 0x0);
				x += 0x8;
		}

		if(x >= 1024)
		{
			y += 0xE;
			x = 0;
		}

		if(y >= 768)
		{
			clear_screen();
		}
	}
}

void clear_screen()
{
	vesa_clear();

	x = 0;
	y = 0;
}

void print_hex(uint8_t h)
{
	char* foo = "00";
	char* hex = "0123456789ABCDEF";
	foo[0] = hex[(h >> 4) & 0xF];
	foo[1] = hex[h & 0xF];
	printf(foo);
}

void print_hex32(uint32_t h)
{
	print_hex((h >> 24) & 0xFF);
	print_hex((h >> 16) & 0xFF);
	print_hex((h >> 8) & 0xFF);
	print_hex( h & 0xFF);
}
