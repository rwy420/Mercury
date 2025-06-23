#include <memory/common.h>
#include <common/screen.h>

void* memset(void* dest, uint8_t val, size_t len) {
    uint8_t* ptr = dest;
    while (len-- > 0)
        *ptr++ = val;
    return dest;
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

void* memcpy(void* dest, const void* src, size_t len) {
    uint8_t* d = dest;
    const uint8_t* s = src;
    while (len-- > 0)
        *d++ = *s++;
    return dest;
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

int atomic_compare_exchange(int* ptr, int* expected, int desired) {
    int success;
    int old_value;

    __asm__ __volatile__(
        "lock cmpxchg %3, %1"
        : "=a"(old_value), "+m"(*ptr)
        : "a"(*expected), "r"(desired)
        : "memory"
    );

    success = (old_value == *expected);
    if (!success) {
        *expected = old_value;
    }

    return success;
}
