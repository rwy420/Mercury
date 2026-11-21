#include <exec/elf32/elf32sym.h>

char* elf32_dynsym_get_lit(uint32_t idx, Elf32_Load_Info* load_info)
{
	Elf32_Sym* dynsym = (Elf32_Sym*) (load_info->offset + load_info->dynsym_off);
	uint32_t dynstr_entry_off = dynsym[idx].st_name;

	return (char*) (load_info->offset + load_info->dynstr_off + dynstr_entry_off);
}
