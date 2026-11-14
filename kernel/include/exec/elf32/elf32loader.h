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

const char* elf32_get_section_name(uint8_t* elf, Elf32_Shdr* shdr, int index);
void* elf32_get_entry(Elf32_Ehdr* hdr, Elf32_Load_Info* load_info);
void elf32_load_segments(Elf32_Ehdr* hdr, Elf32_Phdr* phdr, Elf32_Load_Info* load_info);

void* elf32_load(void* elf_start, uint32_t size, PageDirectory* pd);

#endif
