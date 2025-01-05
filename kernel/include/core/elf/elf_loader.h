#ifndef __MERCURY__CORE__ELF__ELF_LOADER_H
#define __MERCURY__CORE__ELF__ELF_LOADER_H

#include <core/types.h>

#define ELF_IDENT 16

<<<<<<< HEAD
=======
#define EI_MAG0 0x7F
#define EI_MAG1 'E'
#define EI_MAG2 'L'
#define EI_MAG3 'F'

>>>>>>> d183a15 (ELF32 Loader working)
#define SHT_DYNSYM 0xB
#define SHT_SYMTAB 0x2
#define SHT_REL 0x9

#define PT_LOAD 0x1

#define ELF32_R_SYM(INFO)	((INFO) >> 8)
#define ELF32_R_TYPE(INFO)	((uint8_t)(INFO))

<<<<<<< HEAD

=======
>>>>>>> d183a15 (ELF32 Loader working)
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Word;
typedef int32_t Elf32_Sword;

typedef struct
{
	uint8_t e_ident[ELF_IDENT];
	Elf32_Half e_type;
	Elf32_Half e_machine;
	Elf32_Word e_version;
	Elf32_Addr e_entry;
	Elf32_Off e_phoff;
	Elf32_Off e_shoff;
	Elf32_Word e_flags;
	Elf32_Half e_ehsize;
	Elf32_Half e_phentsize;
	Elf32_Half e_phnum;
	Elf32_Half e_shentsize;
	Elf32_Half e_shnum;
	Elf32_Half e_shstrndx;
} __attribute__((packed)) Elf32_Ehdr;

typedef struct {
	Elf32_Word	sh_name;
	Elf32_Word	sh_type;
	Elf32_Word	sh_flags;
	Elf32_Addr	sh_addr;
	Elf32_Off	sh_offset;
	Elf32_Word	sh_size;
	Elf32_Word	sh_link;
	Elf32_Word	sh_info;
	Elf32_Word	sh_addralign;
	Elf32_Word	sh_entsize;
} __attribute__((packed)) Elf32_Shdr;

typedef struct {
    uint32_t   p_type;
    Elf32_Off  p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    uint32_t   p_filesz;
    uint32_t   p_memsz;
	uint32_t   p_flags;
	uint32_t   p_align;
} __attribute__((packed)) Elf32_Phdr;

typedef struct {
	Elf32_Word		st_name;
	Elf32_Addr		st_value;
	Elf32_Word		st_size;
	uint8_t			st_info;
	uint8_t			st_other;
	Elf32_Half		st_shndx;
} __attribute__((packed)) Elf32_Sym;

typedef struct {
	Elf32_Addr		r_offset;
	Elf32_Word		r_info;
} __attribute__((packed)) Elf32_Rel;

int is_image_valid(Elf32_Ehdr* hdr);
void relocate(Elf32_Shdr* shdr, const Elf32_Sym* syms, const char* strings, const char* src, char* dst);
int find_global_symbol_table(Elf32_Ehdr* hdr, Elf32_Shdr* shdr);
int find_symbol_table(Elf32_Ehdr* hdr, Elf32_Shdr* shdr);
void* find_sym(const char* name, Elf32_Shdr* shdr, Elf32_Shdr* shdr_sym, const char* src, char* dst);
void* image_load(char *elf_start, unsigned int size);

/*int main(int argc, char** argv, char** envp)
{
    char buf[1048576]; // Allocate 1MB for the program
    memset(buf, 0x0, sizeof(buf));

    FILE* elf = fopen(argv[1], "rb");

    if (elf != NULL)
    {
        int (*ptr)(int, char **, char**);

        fread(buf, sizeof(buf), 1, elf);
        ptr = image_load(buf, sizeof(buf));

        if (ptr != NULL)
        {
            printf("Run the loaded program:\n");

            // Run the main function of the loaded program
            ptr(argc, argv, envp);
        }
        else
        {
            printf("Loading unsuccessful...\n");
        }

        fclose(elf);

        return 0;
    }
    
    return 1;
}*/

#endif
