#ifndef __MERCURY__EXEC__ELF32__ELF32SYM_H
#define __MERCURY__EXEC__ELF32__ELF32SYM_H

#include <exec/elf32/elf32loader.h>

char* elf32_dynsym_get_lit(uint32_t idx, Elf32_Load_Info* load_info);

#endif
