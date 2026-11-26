#ifndef __MERCURY__EXEC__ELF32__ELF32HASH_H
#define __MERCURY__EXEC__ELF32__ELF32HASH_H

#include <exec/elf32/elf32loader.h>
#include <common/types.h>

typedef struct
{
	uint32_t nbucket;
	uint32_t nchain;
	uint32_t table[];
} Elf32_Sysv_Hash;

uint32_t elf32_sysv_hash(const char* name);
Elf32_Sym* elf32_hash_lookup(const char* name, Elf32_Load_Info* load_info);

#endif
