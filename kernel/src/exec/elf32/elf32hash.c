#include <exec/elf32/elf32hash.h>

uint32_t elf32_sysv_hash(char* name)
{
	uint32_t h = 0;
	uint32_t g;

	while(*name)
	{
		h = (h << 4) + *name++;

		g = h & 0xF0000000;
		if(g) h ^= g >> 24;

		h &= ~g;
	}

	return h;
}
