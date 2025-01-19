#ifndef __MERCURY__EXEC__ELF__SYMTABLE_H
#define __MERCURY__EXEC__ELF__SYMTABLE_H

#include <core/screen.h>

typedef struct
{
	const char* name;
	void* address;
} Symbol;

void* resolve_symbol(const char* name);

#endif
