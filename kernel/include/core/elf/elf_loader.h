#ifndef __MERCURY__CORE__ELF__ELF_LOADER_H
#define __MERCURY__CORE__ELF__ELF_LOADER_H

#include <core/types.h>
#include <core/elf/elf.h>

bool is_image_valid(Elf32_Ehdr* hdr);
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
