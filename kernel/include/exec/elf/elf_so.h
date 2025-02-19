#ifndef __MERCURY__EXEC__ELF__ELF_SO_H
#define __MERCURY__EXEC__ELF__ELF_SO_H

#include <common/types.h>
#include <exec/elf/elf.h>

typedef struct
{
	void* buffer;
	int fd;
	Elf32_Shdr* shdr;
	int shnum;
} Elf32_DL;

void dl_init();
int dlopen(char* path);
void dlclose(int dl);
void* dlsym(int dl, char* sym);

#endif
