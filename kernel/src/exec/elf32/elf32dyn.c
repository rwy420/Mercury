#include <memory/paging.h>
#include <exec/elf32/elf32dyn.h>
#include <memory/common.h>

extern PageDirectory* g_kernel_pd;

int elf32_handle_ptdynamic(Elf32_Phdr* phdr, Elf32_Load_Info* load_info)
{
	set_pd(load_info->pd);
	Elf32_Dyn* dyn = (Elf32_Dyn*) (load_info->offset + phdr->p_vaddr);
	uint32_t count = phdr->p_memsz / sizeof(Elf32_Dyn);
	uint32_t rel_addr = 0, rel_size = 0, rel_entry_size = 0;

	for(int i = 0; i < count; i++)
	{
		switch(dyn[i].d_tag)
		{
			case DT_NULL:
				// Doesn't seem to always end at the entry at count, therefore we'll exit after a DT_NULL entry
				goto pt_dynamic_end;
				break;
			case DT_REL:
				rel_addr = dyn[i].d_un.d_ptr;
				break;
			case DT_RELSZ:
				rel_size = dyn[i].d_un.d_val;
				break;
			case DT_RELENT:
				rel_entry_size = dyn[i].d_un.d_val;
				break;
		}
	}

pt_dynamic_end:
	if(!rel_addr || !rel_size || !rel_entry_size) goto error;
	if(rel_entry_size != sizeof(Elf32_Rel)) goto error;
	if(rel_size % rel_entry_size != 0) goto error;

	Elf32_Rel* relocations = (Elf32_Rel*) (load_info->offset + rel_addr);
	uint32_t relocation_count = rel_size / rel_entry_size;

	for(uint32_t i = 0; i < relocation_count; i++)
	{
		elf32_ptdynamic_relocate(&relocations[i], load_info);
	}

	set_pd(g_kernel_pd);
	return 0;

error:
	set_pd(g_kernel_pd);
	return -1;
}

int elf32_ptdynamic_relocate(Elf32_Rel* rel, Elf32_Load_Info* load_info)
{
	return 0;
}
