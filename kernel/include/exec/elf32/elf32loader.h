#ifndef __MERCURY__EXEC__ELF32__ELF32LOADER_H
#define __MERCURY__EXEC__ELF32__ELF32LOADER_H

#include <memory/paging.h>
#include <common/types.h>
#include <exec/elf32/elf32.h>

typedef struct Elf32_Load_Info
{
	void* image;
	void* entry;
	uint32_t offset;
	uint32_t next_obj; // Only for parent executables to help place the dynlibs
	void* end;
	PageDirectory* pd;

	uint32_t dynstr_off;
	uint32_t dynsym_off;
	Elf32_Rel* relocations,* jmp_relocations;
	uint32_t rel_size, jmprel_size;


	struct Elf32_Load_Info* parent;
	struct Elf32_Load_Info* needed[16];
	uint32_t needed_count;
} Elf32_Load_Info;

void elf32_get_sections(Elf32_Ehdr* ehdr, Elf32_Shdr* shdr, Elf32_Load_Info* load_info);
void* elf32_get_entry(Elf32_Ehdr* hdr, Elf32_Load_Info* load_info);
uint32_t elf32_load_segments(Elf32_Ehdr* hdr, Elf32_Phdr* phdr, Elf32_Load_Info* load_info);

Elf32_Load_Info* elf32_load(void* elf_start, uint32_t size, PageDirectory* pd, Elf32_Load_Info* parent);

#endif
