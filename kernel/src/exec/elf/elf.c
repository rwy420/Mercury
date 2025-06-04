#include <exec/elf/elf.h>

int is_elf_image(Elf32_Ehdr *hdr)
{
	if(hdr->e_ident[0] == EI_MAG0 && hdr->e_ident[1] == EI_MAG1 && hdr->e_ident[2] == EI_MAG2 &&
			hdr->e_ident[3] == EI_MAG3) return true;

	return false;
}

const char* get_section_name(const uint8_t* elf, Elf32_Shdr* shdr, int index)
{
	Elf32_Ehdr* ehdr = (Elf32_Ehdr*) elf;
	Elf32_Shdr* shstrtab_shdr = &shdr[ehdr->e_shstrndx];

	const char* shstrtab = (const char*)(elf + shstrtab_shdr->sh_offset);

	return &shstrtab[shdr[index].sh_name];
}
