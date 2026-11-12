#ifndef __MERCURY__EXEC__ELF32__ELF32LOADER_H
#define __MERCURY__EXEC__ELF32__ELF32LOADER_H

#include <common/types.h>
#include <exec/elf32/elf32.h>

void relocate(Elf32_Shdr* shdr, const Elf32_Sym* syms, const char* strings, const char* src, char* dst);
int find_global_symbol_table(Elf32_Ehdr* hdr, Elf32_Shdr* shdr);
int find_symbol_table(Elf32_Ehdr* hdr, Elf32_Shdr* shdr);
void* find_sym(const char* name, Elf32_Shdr* shdr, Elf32_Shdr* shdr_sym, const char* src, char* dst);
void* image_load(uint8_t* elf_start, uint32_t size, int debug);

#endif
