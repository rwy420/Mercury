#include <exec/elf/elf_dl.h>
#include <exec/elf/elf.h>
#include <fs/fat16/fat16.h>
#include <common/screen.h>
#include <memory/common.h>
#include <memory/heap.h>

Elf32_DL dynamic_libraries[0x10];
int dl_count;

void dl_init()
{
	dl_count = 0;
}

int dlopen(char* path)
{
	int fd = fat16_open(path, 'r');
	uint32_t size = fat16_size("/LIB/LIBC.SO");
	uint8_t* buffer = kmalloc(size);
	fat16_read(fd, buffer, size);

	Elf32_Ehdr* hdr = (Elf32_Ehdr*) buffer;
	Elf32_Shdr* shdr = (Elf32_Shdr*)(buffer + hdr->e_shoff);

	int dl = dl_count;
	dl_count++;
	dynamic_libraries[dl].buffer = buffer;
	dynamic_libraries[dl].fd = fd;
	dynamic_libraries[dl].shdr = shdr;
	dynamic_libraries[dl].shnum = 20;

	return dl;
}

void dlclose(int dl)
{
	kfree(dynamic_libraries[dl].buffer);
	fat16_close(dynamic_libraries[dl].fd);
	dl_count--;
}

void* dlsym(int dl, char* sym)
{
	int shnum = dynamic_libraries[dl].shnum;
	Elf32_Shdr* shdr = dynamic_libraries[dl].shdr;
	char* elf = dynamic_libraries[dl].buffer;

	Elf32_Shdr* dynsym = NULL_PTR, *dynstr = NULL_PTR;
	for(int i = 0; i < shnum; i++)
	{
		if(strcmp(get_section_name(elf, shdr, i), ".dynsym") == 0)
		{
			dynsym = &shdr[i];
		}
		else if(strcmp(get_section_name(elf, shdr, i), ".dynstr") == 0)
		{
			dynstr = &shdr[i];
		}
	}

	if(!dynsym || !dynstr)
	{
		return NULL_PTR;
	}

	Elf32_Sym* symbols = (Elf32_Sym*) (elf + dynsym->sh_offset);
	int num_sym = dynsym->sh_size / sizeof(Elf32_Sym);
	char* strtab = (char*) (elf + dynstr->sh_offset);

	for(int i = 0; i < num_sym; i++)
	{
		if(strcmp(sym, strtab + symbols[i].st_name) == 0)
		{
			return (void*) elf + symbols[i].st_value;
		}
	}

	return NULL_PTR;
}
