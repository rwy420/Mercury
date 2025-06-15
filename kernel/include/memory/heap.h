#ifndef __MERCURY__MEMORY__HEAP_H
#define __MERCURY__MEMORY__HEAP_H

#include <common/types.h>

typedef struct MemoryChunk
{
	struct MemoryChunk* next;
    struct MemoryChunk* prev;
    int allocated;
    size_t size;
} __attribute__((packed)) MemoryChunk;

uint32_t heap_init(size_t start, size_t size);
void* kmalloc(size_t size);
void kfree(void* pointer);

void* kmalloc_aligned(size_t alignment, size_t size);
void kfree_aligned(void* pointer);

#endif
