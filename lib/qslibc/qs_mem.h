#ifndef __QSLBIC__QS_MEM_H
#define __QSLBIC__QS_MEM_H

__attribute__((weak)) void* kmalloc(unsigned int size);
__attribute__((weak)) void kfree(void* pointer);
__attribute__((weak)) void* kmalloc_aligned(unsigned int alignment, unsigned int size);
__attribute__((weak)) void kfree_aligned(void* pointer);

__attribute__((weak)) void* memset(void* buffer, unsigned char byte, unsigned int length);
__attribute__((weak)) void* memmove(void* dest, const void* src, unsigned int length);

#endif
