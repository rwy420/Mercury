#include <exec/elf/symtable.h>
#include <exec/elf/elf_loader.h>
#include <memory/common.h>
#include <common/screen.h>
#include <memory/heap.h>
#include <hardware/port.h>

Symbol* symtable;
uint32_t symbol_idx;

void symtable_init()
{
	symtable = kmalloc(sizeof(Symbol) * MAX_SYMBOLS);
	symbol_idx = 0;
}

void register_symbol(string name, void* address)
{
	symtable[symbol_idx].name = name;
	symtable[symbol_idx].address = address;
	symbol_idx++;
}

void* resolve_symbol(const char* name)
{
	for(uint32_t i = 0; i < symbol_idx; i++)
	{
		if(!strcmp(symtable[i].name, name))
		{
			return symtable[i].address;
		}
	}

	return NULL_PTR; 
}
