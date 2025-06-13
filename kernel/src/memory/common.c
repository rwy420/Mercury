#include <memory/common.h>
#include <common/screen.h>

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

void memcpy(void* dest, void* src, size_t n)
{
	uint8_t* csrc = (uint8_t*) src;
	uint8_t* cdest = (uint8_t*) dest;

	for(uint32_t i = 0; i < n; i++)
	{
		cdest[i] = csrc[i];
	}
}

int strcmp(const char *a, const char *b)
{
    while (*a && *a == *b) { ++a; ++b; }
    return ((int)(unsigned char)(*a) - (int)(unsigned char)(*b));
}

int memcmp(const void* s1, const void* s2, uint32_t size)
{
	const uint8_t* p1 = (const uint8_t*) s1;
	const uint8_t* p2 = (const uint8_t*) s2;

	for(uint32_t i = 0; i < size; i++)
	{
		if(p1[i] != p2[i]) return p1[i] - p2[i];
	}

	return 0;
}
