#include <exec/elf/symtable.h>
#include <memory/common.h>
#include <core/screen.h>
#include <memory/mem_manager.h>

Symbol* symtable;
uint32_t symbol_idx;

void init_symtable()
{
	symtable = malloc(sizeof(Symbol) * MAX_SYMBOLS);
	symbol_idx = 0;

	register_symbol("printf", printf);
	register_symbol("clear_screen", clear_screen);
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
	printf(name);
	
	return NULL_PTR; 
}
