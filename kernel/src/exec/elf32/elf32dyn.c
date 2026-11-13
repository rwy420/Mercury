#include <memory/paging.h>
#include <exec/elf32/elf32dyn.h>
#include <memory/common.h>

extern PageDirectory* g_kernel_pd;

int elf32_handle_ptdynamic(Elf32_Phdr* phdr, Elf32_Load_Info* load_info)
{
	set_pd(load_info->pd);
	Elf32_Dyn* dyn = (Elf32_Dyn*) (load_info->offset + phdr->p_vaddr);
	uint32_t count = phdr->p_memsz / sizeof(Elf32_Dyn);

	for(int i = 0; i < count; i++)
	{
		switch(dyn[i].d_tag)
		{
			case DT_NULL:
				// Doesn't seem to always end at the entry at count, therefore we'll exit after a DT_NULL entry
				goto pt_dynamic_end;
				break;
			case DT_REL:
				break;
		}
	}

pt_dynamic_end:


	set_pd(g_kernel_pd);

	return 0;
}
