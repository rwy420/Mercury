#ifndef __MERCURY__EXEC__ELF__SYMTABLE_H
#define __MERCURY__EXEC__ELF__SYMTABLE_H

#include <common/screen.h>

#define MAX_SYMBOLS 256

typedef struct
{
	string name;
	void* address;
} Symbol;

void init_symtable();
void register_symbol(string name, void* address);
void* resolve_symbol(const char* name);

#endif
