#include <exec/elf/symtable.h>
#include <memory/common.h>
#include <core/screen.h>

void zero()
{
}

void* resolve_symbol(const char* name)
{
	if(!strcmp("printf", name)) return (void*) printf;

	return (void*) zero;
}
