#ifndef __QS__EXEC__ELF__SYMTABLE_H
#define __QS__EXEC__ELF__SYMTABLE_H

#include <core/screen.h>

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
