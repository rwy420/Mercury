#ifndef __MERCURY__EXEC__ELF32__ELF32DYNLIB_H
#define __MERCURY__EXEC__ELF32__ELF32DYNLIB_H

#include <exec/elf32/elf32loader.h>

Elf32_Load_Info* elf32_handle_dtneeded(const char* nameoff, Elf32_Load_Info* load_info);
void* elf32_total_lookup(const char* name, Elf32_Load_Info* load_info); // Lookup on all loaded libs
void* elf32_lib_lookup(const char* name, Elf32_Load_Info* load_info); // Lookup on specified lib

#endif
