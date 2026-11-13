#ifndef __MERCURY__EXEC__ELF32__ELF32LOADER_H
#define __MERCURY__EXEC__ELF32__ELF32LOADER_H

#include <memory/paging.h>
#include <common/types.h>
#include <exec/elf32/elf32.h>

typedef struct
{
	void* image;
	void* entry;
	uint32_t offset;
	void* end;
	PageDirectory* pd;
} Elf32_Load_Info;

void elf32_relocate(Elf32_Shdr* shdr, Elf32_Sym* syms, char* strings, char* src, char* dst);
int elf32_find_global_symbol_table(Elf32_Ehdr* hdr, Elf32_Shdr* shdr);
int elf32_find_symbol_table(Elf32_Ehdr* hdr, Elf32_Shdr* shdr);
void* elf32_find_sym(char* name, Elf32_Shdr* shdr, Elf32_Shdr* shdr_sym, char* src, char* dst);
const char* elf32_get_section_name(uint8_t* elf, Elf32_Shdr* shdr, int index);
void* elf32_get_entry(Elf32_Ehdr* hdr, Elf32_Load_Info* load_info);
void elf32_load_segments(Elf32_Ehdr* hdr, Elf32_Phdr* phdr, Elf32_Load_Info* load_info);

void* elf32_load(void* elf_start, uint32_t size, PageDirectory* pd);

#endif
