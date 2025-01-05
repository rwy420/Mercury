#ifndef __MERCURY__MEMORY__KHEAP_H
#define __MERCURY__MEMORY__KHEAP_H

#include <core/types.h>

void set_block(uint32_t bit);
void unset_block(uint32_t bit);

int32_t find_first_free_blocks(uint32_t num_blocks);

void init_memory_manager(uint32_t start_address, uint32_t size);

void init_memory_region(uint32_t base_address, uint32_t size);
void deinit_memory_region(uint32_t base_address, uint32_t size);

uint32_t* alloc_blocks(uint32_t num_blocks);
void free_blocks(uint32_t* address, uint32_t num_blocks);

void heap_init(size_t start, size_t size);
void* malloc(size_t size);
void free(void* pointer);

#endif
