#include "multiboot.h"
#include <fs/initrd.h>

void* initrd;

int load_initrd()
{
	multiboot_module_t* initrd_mod = mb_get_module(0);
	initrd = (void*) initrd_mod->mod_start;

	return 0;
}
