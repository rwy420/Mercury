#ifndef __QSLBIC__QS_ELF_H
#define __QSLBIC__QS_ELF_H

__attribute__((weak)) void* image_load(char* elf_start, unsigned int size, int debug);

#endif 
