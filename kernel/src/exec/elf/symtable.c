#include <exec/elf/symtable.h>
#include <memory/common.h>
#include <core/screen.h>
#include <memory/mem_manager.h>
#include <hardware/port.h>

Symbol* symtable;
uint32_t symbol_idx;

void init_symtable()
{
	symtable = malloc(sizeof(Symbol) * MAX_SYMBOLS);
	symbol_idx = 0;

	//qslibc/qs_io.h
	register_symbol("inb", inb);
	register_symbol("outb", outb);
	register_symbol("outb_slow", outb_slow);
	register_symbol("inw", inw);
	register_symbol("outw", outw);
	register_symbol("inl", inl);
	register_symbol("outl", outl);
	//qslibc/qs_log.h
	register_symbol("kprintf", printf);
	register_symbol("kscreen_clear", clear_screen);
	//qslibc/qs_mem.h
	register_symbol("kmalloc", malloc);
	register_symbol("kfree", free);
	register_symbol("kmalloc_aligned", malloc_aligned);
	register_symbol("kfree_aligned", free_aligned);
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
