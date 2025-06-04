#ifndef __MERCURY__MEMORY__KHEAP_H
#define __MERCURY__MEMORY__KHEAP_H

#include <common/types.h>

void set_block(uint32_t bit);
void unset_block(uint32_t bit);

int32_t find_first_free_blocks(uint32_t num_blocks);

void init_memory_manager(uint32_t start_address, uint32_t size);

void init_memory_region(uint32_t base_address, uint32_t size);
void deinit_memory_region(uint32_t base_address, uint32_t size);

uint32_t* alloc_blocks(uint32_t num_blocks);
void free_blocks(uint32_t* address, uint32_t num_blocks);

void heap_init(size_t start, size_t size);
void* kmalloc(size_t size);
void kfree(void* pointer);

void* kmalloc_aligned(size_t alignment, size_t size);
void kfree_aligned(void* pointer);

#endif
