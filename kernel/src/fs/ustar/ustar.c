#include <fs/ustar/ustar.h>

USTarFile* ustar_dump_archive(void* archive)
{

}

// From https://wiki.osdev.org/USTAR
uint32_t oct_to_bin(char* str, int size)
{
    int n = 0;
	unsigned char *c = str;
    while (size-- > 0) {
        n *= 8;
        n += *c - '0';
        c++;
    }
    return n;
}
