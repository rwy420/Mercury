#ifndef __MERCURY__EXEC__ELF32__ELF32SYM_H
#define __MERCURY__EXEC__ELF32__ELF32SYM_H

#include <exec/elf32/elf32.h>

int elf32_find_global_symbol_table(Elf32_Ehdr* hdr, Elf32_Shdr* shdr);
int elf32_find_symbol_table(Elf32_Ehdr* hdr, Elf32_Shdr* shdr);
void* elf32_find_sym(char* name, Elf32_Shdr* shdr, Elf32_Shdr* shdr_sym, char* src, char* dst);

#endif
