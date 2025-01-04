#include <memory/common.h>
#include <core/screen.h>

extern uint32_t kernel_start_address;
extern uint32_t kernel_end_address;
extern uint32_t kernel_size;

void* memset(void* buffer, uint8_t byte, uint32_t length)
{
	uint8_t* pointer = (uint8_t*) buffer;

	for(uint32_t i = 0; i < length; i++) pointer[i] = byte;

	return buffer;
}

void* memmove(void* dest, const void* src, size_t len)
{
	char* d = dest;
	const char* s = src;
	if (d < s)
		while (len--)
			*d++ = *s++;
	else
    {
		char* lasts = s + (len-1);
		char* lastd = d + (len-1);
		while (len--)
			*lastd-- = *lasts--;
    }
	return dest;
}

bool strcmp(const char *a, const char *b)
{
    while (*a && *a == *b) { ++a; ++b; }
    return (int)(unsigned char)(*a) - (int)(unsigned char)(*b);
}

void print_memory_info()
{
	printf("<Mercury> Kernel start: 0x");
	print_hex((kernel_start_address >> 24) & 0xFF);
	print_hex((kernel_start_address >> 16) & 0xFF);
	print_hex((kernel_start_address >> 8) & 0xFF);
	print_hex(kernel_start_address & 0xFF);
	printf("\n");

	printf("<Mercury> Kernel end: 0x");
	print_hex((kernel_end_address >> 24) & 0xFF);
	print_hex((kernel_end_address >> 16) & 0xFF);
	print_hex((kernel_end_address >> 8) & 0xFF);
	print_hex(kernel_end_address & 0xFF);
	printf("\n");

	printf("<Mercury> Kernel size: 0x");
	print_hex((kernel_size >> 24) & 0xFF);
	print_hex((kernel_size >> 16) & 0xFF);
	print_hex((kernel_size >> 8) & 0xFF);
	print_hex(kernel_size & 0xFF);
	printf("\n");
}
