#include <exec/elf32/elf32dynlib.h>
#include <printf.h>
#include <memory/common.h>

int elf32_handle_dtneeded(char* nameoff, Elf32_Load_Info* load_info)
{
	char* name = (char*) (load_info->offset + load_info->dynstr_off + (uint32_t) nameoff);
	size_t name_len = strlen(name);
	char path[5 + name_len]; // /lib/ + strlen(name);
	memcpy(path, "/lib/", 5);
	memcpy(&path[5], name, name_len + 1/* Should include \0*/); 

	kprintf("%s\n", path);

	return 0;
}
