#ifndef __MERCURY__CORE__ELF__ELF_LOADER_H
#define __MERCURY__CORE__ELF__ELF_LOADER_H

#include <core/types.h>
#include <core/elf/elf.h>

void relocate(Elf32_Shdr* shdr, const Elf32_Sym* syms, const char* strings, const char* src, char* dst);
int find_global_symbol_table(Elf32_Ehdr* hdr, Elf32_Shdr* shdr);
int find_symbol_table(Elf32_Ehdr* hdr, Elf32_Shdr* shdr);
void* find_sym(const char* name, Elf32_Shdr* shdr, Elf32_Shdr* shdr_sym, const char* src, char* dst);
void* image_load(char *elf_start, unsigned int size, bool debug);

#endif
