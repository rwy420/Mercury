#include <exec/elf32/elf32dyn.h>
#include <memory/common.h>

void elf32_dyn_load_segments(Elf32_Ehdr* hdr, Elf32_Phdr* phdr, Elf32_Load_Info* load_info)
{
	for (int i = 0; i < hdr->e_phnum; ++i)
	{
		if (phdr[i].p_type != PT_LOAD)
	    {
		    continue;
	    }

		if (!phdr[i].p_filesz)
		{
			continue;
		}

		void* segment = load_info->image + phdr[i].p_offset;
		void* addr = load_info->base + phdr[i].p_vaddr;
		memmove(addr, segment, phdr[i].p_filesz);
	}
}
