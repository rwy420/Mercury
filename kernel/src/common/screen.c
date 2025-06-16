#include <common/screen.h>
#include <vesa.h>

static uint16_t x = 0, y = 0;

int syscall_printf(void* buffer, size_t length)
{
	printf((string) buffer);
	return 0;
}

void printf_color(string str, uint32_t fg, uint32_t bg)
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
				vesa_putc(str[i], x, y, fg, bg);
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

void printf(string str)
{
	printf_color(str, COLOR_WHITE, COLOR_BLACK);
}

void terminal_move_left()
{
	x -= 8;
	printf(" ");
	x -= 8;
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

void print_uint8_t(uint8_t value)
{
	uint8_t buffer[4];
	uint8_t_to_string(value, buffer);
	printf(buffer);
}

void print_uint32_t(uint32_t value)
{
	uint8_t buffer[11];
	uint32_t_to_string(value, buffer);
	printf(buffer);
}

void uint8_t_to_string(uint8_t value, char* buffer)
{
	int i = 0;
	do
	{
		buffer[i++] = '0' + (value % 10);
		value /= 10;
	} while(value > 0);
	buffer[i] = '\0';

	for(int j = 0, k = i - 1; j < k; j++, k--)
	{
		char temp = buffer[j];
		buffer[j] = buffer[k];
		buffer[k] = temp;
	}
}

void uint32_t_to_string(uint32_t value, char *buffer)
{
    char temp[11];
    int i = 0;

    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    while (value > 0) {
        temp[i++] = '0' + (value % 10);
        value /= 10;
    }

    int j = 0;
    while (i > 0) {
        buffer[j++] = temp[--i];
    }

    buffer[j] = '\0';
}
