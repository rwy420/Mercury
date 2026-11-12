#ifndef __MERCURY__EXEC__ELF32__ELF32DYN_H
#define __MERCURY__EXEC__ELF32__ELF32DYN_H

#include <exec/elf32/elf32loader.h>
#include <exec/elf32/elf32.h>

void elf32_dyn_load_segments(Elf32_Ehdr* hdr, Elf32_Phdr* phdr, Elf32_Load_Info* load_info);

#endif
