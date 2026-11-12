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

void elf32_relocate(Elf32_Shdr* shdr, const Elf32_Sym* syms, const char* strings, const char* src, char* dst)
{
    Elf32_Rel* rel = (Elf32_Rel*)(src + shdr->sh_offset);

    for(int j = 0; j < shdr->sh_size / sizeof(Elf32_Rel); j += 1)
    {
        const char* sym = strings + syms[ELF32_R_SYM(rel[j].r_info)].st_name;
		
        switch (ELF32_R_TYPE(rel[j].r_info))
        {
            case R_386_JMP_SLOT:
				//*(Elf32_Word*)(dst + rel[j].r_offset) = (Elf32_Word) resolve_symbol(sym);
            case R_386_GLOB_DAT:
				//*(Elf32_Word*)(dst + rel[j].r_offset) = (Elf32_Word) dlsym(dl, sym);
                break;
        }
    }
}

int elf32_find_global_symbol_table(Elf32_Ehdr* hdr, Elf32_Shdr* shdr)
{
    for (int i = 0; i < hdr->e_shnum; i++)
    {
        if (shdr[i].sh_type == SHT_DYNSYM)
        {
            return i;
            break;
        }
    }

    return -1;
}

int elf32_find_symbol_table(Elf32_Ehdr* hdr, Elf32_Shdr* shdr)
{
    for (int i = 0; i < hdr->e_shnum; i++)
    {
			print_uint8_t(shdr[i].sh_type);
			printf("\n");

        if (shdr[i].sh_type == SHT_SYMTAB)
        {
            return i;
            break;
        }
    }

    return -1;
}

void* elf32_find_sym(const char* name, Elf32_Shdr* shdr, Elf32_Shdr* shdr_sym, const char* src, char* dst)
{
    Elf32_Sym* syms = (Elf32_Sym*)(src + shdr_sym->sh_offset);
    const char* strings = src + shdr[shdr_sym->sh_link].sh_offset;
    
    for (int i = 0; i < shdr_sym->sh_size / sizeof(Elf32_Sym); i += 1)
    {
        /*if (strcmp(name, strings + syms[i].st_name) == 0)
        {
            return dst + syms[i].st_value;
        }*/
    }

    return 0;
}

const char* elf32_get_section_name(const uint8_t* elf, Elf32_Shdr* shdr, int index)
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

void* elf32_load(void* elf_start, uint32_t size)
{
	Elf32_Load_Info* load_info = kmalloc(sizeof(Elf32_Load_Info));
    Elf32_Ehdr* hdr = (Elf32_Ehdr*) elf_start;
    Elf32_Phdr* phdr = (Elf32_Phdr *)(elf_start + hdr->e_phoff);
    Elf32_Shdr* shdr = (Elf32_Shdr *)(elf_start + hdr->e_shoff);

    uint8_t* base;

	if (!is_elf_image(hdr))
    {
        printf("<ELF> Invalid ELF image\n");
        return 0;
    }

	if(!load_info) return 0;

	uint32_t num_frames = ((size + FRAME_SIZE) / FRAME_SIZE) + 1;
    base = alloc_frames(num_frames);

	for(int i = 0; i < num_frames; i++)
	{
		void* frame = (base + i * 0x1000);
		map_page_pd_flags(g_kernel_pd, frame, frame, PTE_PRESENT | PTE_RW | PTE_USER);
	}

    if (!base)
    {
        printf("<ELF> Error allocating memory\n");
        return 0;
    }



    memset(base, 0x0, size);
	load_info->base = base;
	load_info->image = elf_start;

   	switch(hdr->e_type)
	{
		case(ET_EXEC):
			load_info->offset = 0;
			break;
		case (ET_DYN):
			load_info->offset = (uint32_t) load_info->base;
			elf32_dyn_load_segments(hdr, phdr, load_info);
			break;
	}


    //int global_symbol_table_index = elf32_find_global_symbol_table(hdr, shdr);
    //Elf32_Sym* global_syms = (Elf32_Sym*)(elf_start + shdr[global_symbol_table_index].sh_offset);
    //char* global_strings = elf_start + shdr[shdr[global_symbol_table_index].sh_link].sh_offset;

    /*for (i = 0; i < hdr->e_shnum; ++i)
    {
        if (shdr[i].sh_type == SHT_REL)
        {
            relocate(shdr + i, global_syms, global_strings, elf_start, exec, dl);
        }
    }*/

	//int symbol_table_index = find_symbol_table(hdr, shdr);
    //print_hex32(symbol_table_index);
	//entry = find_sym("main", shdr, shdr + symbol_table_index, elf_start, exec);

	load_info->entry = elf32_get_entry(hdr, load_info);

	return load_info->entry;
}
