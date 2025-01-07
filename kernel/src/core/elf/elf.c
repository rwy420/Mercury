#include <core/elf/elf.h>

bool is_image_valid(Elf32_Ehdr *hdr)
{
	if(hdr->e_ident[0] == EI_MAG0 && hdr->e_ident[1] == EI_MAG1 &&hdr->e_ident[2] == EI_MAG2 &&
			hdr->e_ident[3] == EI_MAG3) return true;

	return false;
}
