#include <core/elf/elf_loader.h>
#include <memory/common.h>
#include <core/screen.h>
#include <memory/mem_manager.h>

int is_image_valid(Elf32_Ehdr *hdr)
{
    return 1;
}

void relocate(Elf32_Shdr* shdr, const Elf32_Sym* syms, const char* strings, const char* src, char* dst)
{
    Elf32_Rel* rel = (Elf32_Rel*)(src + shdr->sh_offset);

    for(int j = 0; j < shdr->sh_size / sizeof(Elf32_Rel); j += 1)
    {
        const char* sym = strings + syms[ELF32_R_SYM(rel[j].r_info)].st_name;

		/*
        switch (ELF32_R_TYPE(rel[j].r_info))
        {
            case R_386_JMP_SLOT:
            case R_386_GLOB_DAT:
                (Elf32_Word*)(dst + rel[j].r_offset) = (Elf32_Word)resolve(sym);
                break;
        }
		*/
    }
}

int find_global_symbol_table(Elf32_Ehdr* hdr, Elf32_Shdr* shdr)
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

int find_symbol_table(Elf32_Ehdr* hdr, Elf32_Shdr* shdr)
{
    for (int i = 0; i < hdr->e_shnum; i++)
    {
        if (shdr[i].sh_type == SHT_SYMTAB)
        {
            return i;
            break;
        }
    }

    return -1;
}

void* find_sym(const char* name, Elf32_Shdr* shdr, Elf32_Shdr* shdr_sym, const char* src, char* dst)
{
    Elf32_Sym* syms = (Elf32_Sym*)(src + shdr_sym->sh_offset);
    const char* strings = src + shdr[shdr_sym->sh_link].sh_offset;
    
    for (int i = 0; i < shdr_sym->sh_size / sizeof(Elf32_Sym); i += 1)
    {
        if (strcmp(name, strings + syms[i].st_name) == 0)
        {
            return dst + syms[i].st_value;
        }
    }

    return 0;
}

void* image_load(char* elf_start, unsigned int size)
{
    Elf32_Ehdr* hdr;
    Elf32_Phdr* phdr;
    Elf32_Shdr* shdr;
    char* start;
    char* taddr;
    void* entry;
    int i = 0;
<<<<<<< HEAD
    char* exec;

    hdr = (Elf32_Ehdr *) elf_start;
    
    if (!is_image_valid(hdr))
=======
    uint8_t* exec;

    hdr = (Elf32_Ehdr*) elf_start;

	if (!is_image_valid(hdr))
>>>>>>> d183a15 (ELF32 Loader working)
    {
        printf("Invalid ELF image\n");
        return 0;
    }

<<<<<<< HEAD
	
    exec = (uint8_t*) alloc_blocks(size / 4096);
=======
    exec = malloc(size);
>>>>>>> d183a15 (ELF32 Loader working)

    if (!exec)
    {
        printf("Error allocating memory\n");
        return 0;
    }

    memset(exec, 0x0, size);

    phdr = (Elf32_Phdr *)(elf_start + hdr->e_phoff);

    for (i = 0; i < hdr->e_phnum; ++i)
    {
        if (phdr[i].p_type != PT_LOAD)
        {
            continue;
        }

        if (!phdr[i].p_filesz)
        {
            continue;
        }

        start = elf_start + phdr[i].p_offset;
        taddr = phdr[i].p_vaddr + exec;
        memmove(taddr, start, phdr[i].p_filesz);
    }

    shdr = (Elf32_Shdr *)(elf_start + hdr->e_shoff);

    int global_symbol_table_index = find_global_symbol_table(hdr, shdr);
    Elf32_Sym* global_syms = (Elf32_Sym*)(elf_start + shdr[global_symbol_table_index].sh_offset);
    char* global_strings = elf_start + shdr[shdr[global_symbol_table_index].sh_link].sh_offset;
    
    for (i = 0; i < hdr->e_shnum; ++i)
    {
        if (shdr[i].sh_type == SHT_REL)
        {
            relocate(shdr + i, global_syms, global_strings, elf_start, exec);
        }
    }

    int symbol_table_index = find_symbol_table(hdr, shdr);
    entry = find_sym("main", shdr, shdr + symbol_table_index, elf_start, exec);

<<<<<<< HEAD
   return entry;
=======
	return entry;
>>>>>>> d183a15 (ELF32 Loader working)
}
