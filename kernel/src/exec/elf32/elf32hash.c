#include "exec/elf32/elf32sym.h"
#include "printf.h"
#include <exec/elf32/elf32hash.h>
#include <memory/common.h>

uint32_t elf32_sysv_hash(char* name)
{
	uint32_t h = 0;
	uint32_t g;

	while(*name)
	{
		h = (h << 4) + *name++;

		g = h & 0xF0000000;
		if(g) h ^= g >> 24;

		h &= ~g;
	}

	return h;
}

Elf32_Sym* elf32_hash_lookup(char* name, Elf32_Load_Info* load_info)
{
	uint32_t hash = elf32_sysv_hash(name);
	Elf32_Sysv_Hash* dthash = (Elf32_Sysv_Hash*) (load_info->offset + load_info->hash_off);
	uint32_t* buckets = dthash->table;
	uint32_t* chains = &dthash->table[dthash->nbucket];

	uint32_t bucket = hash % dthash->nbucket;	
	uint32_t index = buckets[bucket];

	while(index != 0)
	{
		Elf32_Sym* dynsym = &((Elf32_Sym*) (load_info->offset + load_info->dynsym_off))[index];
		char* symlit = (char*) (load_info->offset + load_info->dynstr_off + dynsym->st_name);
	
		if(!strcmp(name, symlit))
		{
			return dynsym;
		}

		index = chains[index];
	}

	return 0;
}
