#ifndef __MERCURY__EXEC__ELF32__ELF32DYN_H
#define __MERCURY__EXEC__ELF32__ELF32DYN_H

#include <exec/elf32/elf32loader.h>
#include <exec/elf32/elf32.h>

int elf32_handle_ptdynamic(Elf32_Phdr* phdr, Elf32_Load_Info* load_info);
int elf32_ptdynamic_relocate(Elf32_Rel* rel, Elf32_Load_Info* load_info);

#endif
