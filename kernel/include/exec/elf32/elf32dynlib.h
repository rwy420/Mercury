#ifndef __MERCURY__EXEC__ELF32__ELF32DYNLIB_H
#define __MERCURY__EXEC__ELF32__ELF32DYNLIB_H

#include <exec/elf32/elf32loader.h>

int elf32_handle_dtneeded(char* nameoff, Elf32_Load_Info* load_info);

#endif
