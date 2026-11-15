#include <exec/elf32/elf32dynlib.h>
#include <printf.h>

int elf32_handle_dtneeded(char* nameoff, Elf32_Load_Info* load_info)
{
	char* name = (char*) (load_info->offset + load_info->dynstr_off + (uint32_t) nameoff);
	kprintf("DT_NEEDED: %s\n", name);

	return 0;
}
