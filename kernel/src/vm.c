#include <vm.h>

void dbg_printf(char* str)
{
	for(int i = 0; str[i] != '\0'; i++)
	{
		dbg_putc(str[i]);
	}
}
