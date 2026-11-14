#include "exec/elf32/elf32dyn.h"
#include <exec/elf32/elf32loader.h>
#include <exec/elf32/elf32.h>
#include <exec/elf32/elf32dynlib.h>
#include <memory/common.h>
#include <common/screen.h>
#include <memory/heap.h>
#include <memory/frames.h>
#include <memory/paging.h>

extern PageDirectory* g_kernel_pd;

const char* elf32_get_section_name(uint8_t* elf, Elf32_Shdr* shdr, int index)
{
	Elf32_Ehdr* ehdr = (Elf32_Ehdr*) elf;
	Elf32_Shdr* shstrtab_shdr = &shdr[ehdr->e_shstrndx];

	const char* shstrtab = (const char*)(elf + shstrtab_shdr->sh_offset);

	return &shstrtab[shdr[index].sh_name];
}

void* elf32_get_entry(Elf32_Ehdr* hdr, Elf32_Load_Info* load_info)
{
	return (void*) hdr->e_entry + load_info->offset;
}

void elf32_load_segments(Elf32_Ehdr* hdr, Elf32_Phdr* phdr, Elf32_Load_Info* load_info)
{
	// Determine total size for allocation
	uint32_t min_vaddr = 0xFFFFFFFF, max_vaddr = 0;

	for(int i = 0; i < hdr->e_phnum; i++)
	{
		if(phdr[i].p_type != PT_LOAD) continue;
		if(phdr[i].p_vaddr < min_vaddr) min_vaddr = phdr[i].p_vaddr;
		else if((phdr[i].p_vaddr + phdr[i].p_memsz) > max_vaddr) max_vaddr = phdr[i].p_vaddr + phdr[i].p_memsz;
	}

	min_vaddr = ALIGN_DOWN(min_vaddr, FRAME_SIZE);
	max_vaddr = ALIGN_UP(max_vaddr, FRAME_SIZE);

	uint32_t total_size = max_vaddr - min_vaddr;

	uint32_t num_frames = ALIGN_UP(total_size, PAGE_SIZE) / PAGE_SIZE;
	void* frames = alloc_frames(num_frames); //TODO: error handling if allocation faile

	for(int i = 0; i < num_frames; i++)
	{
		void* paddr = (void*)(frames + i * FRAME_SIZE);
		void* vaddr = (void*)(min_vaddr + load_info->offset + i * FRAME_SIZE);
		map_page_pd_flags(load_info->pd, paddr, vaddr, PDE_PRESENT | PDE_RW | PDE_USER);
	}

	for (int i = 0; i < hdr->e_phnum; ++i)
	{
		if (!phdr[i].p_filesz)
		{
			continue;
		}

		void* psegment = (void*) virtual_to_physical((void*)(load_info->image + phdr[i].p_offset));
		void* vaddr = (void*)(load_info->offset + phdr[i].p_vaddr);

		switch(phdr[i].p_type)
		{
			case PT_LOAD:
				set_pd(load_info->pd);
				memmove(vaddr, psegment, phdr[i].p_filesz);
				uint32_t zeromem_size = phdr[i].p_memsz - phdr[i].p_filesz;
				if(zeromem_size > 0) memset((void*) vaddr + phdr[i].p_filesz, 0, zeromem_size);
				set_pd(g_kernel_pd);
				break;
			case PT_DYNAMIC:
				elf32_handle_ptdynamic(&phdr[i], load_info); //TODO: error handling
				break;
			default:
				break;
		}
	}
}

void* elf32_load(void* elf_start, uint32_t size, PageDirectory* pd)
{
	Elf32_Load_Info* load_info = kmalloc(sizeof(Elf32_Load_Info));
    Elf32_Ehdr* hdr = (Elf32_Ehdr*) elf_start;
    Elf32_Phdr* phdr = (Elf32_Phdr*)(elf_start + hdr->e_phoff);
    Elf32_Shdr* shdr = (Elf32_Shdr*)(elf_start + hdr->e_shoff);

	if (!is_elf_image(hdr))
    {
        printf("<ELF> Invalid ELF image\n");
        return 0;
    }

	if(!load_info) return 0;

	load_info->image = elf_start;
	load_info->pd = pd;

   	switch(hdr->e_type)
	{
		case(ET_EXEC):
			load_info->offset = 0;
			break;
		case (ET_DYN):
			load_info->offset = 0x60000000;
			break;
	}

	elf32_load_segments(hdr, phdr, load_info);

    /*int global_symbol_table_index = elf32_find_global_symbol_table(hdr, shdr);
    Elf32_Sym* global_syms = (Elf32_Sym*)(elf_start + shdr[global_symbol_table_index].sh_offset);
    char* global_strings = elf_start + shdr[shdr[global_symbol_table_index].sh_link].sh_offset;

    for (int i = 0; i < hdr->e_shnum; ++i)
    {
        if (shdr[i].sh_type == SHT_REL)
        {
            elf32_relocate(shdr + i, global_syms, global_strings, elf_start, elf_start);
        }
    }*/

	load_info->entry = elf32_get_entry(hdr, load_info);

	return load_info->entry;
}
