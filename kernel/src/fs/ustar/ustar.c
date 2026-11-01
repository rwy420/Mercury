#include <fs/ustar/ustar.h>
#include <memory/common.h>
#include <common/screen.h>

USTARFile* ustar_dump_archive(void* archive)
{
	void* ptr = archive;
	USTARNode* current = ptr;

	while(!memcmp(current->indicator, "ustar", 5))
	{
		uint32_t size = oct_to_bin((char*) current->size_in_bytes, 11);
		printf(current->name);
		printf(" ");
		print_uint32_t(size);
		printf("\n");

		ptr += 512 + ((size + 511) & ~511);
		current = ptr;
	}

	return 0;
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
