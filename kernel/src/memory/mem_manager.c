#include <memory/mem_manager.h>

#include <core/screen.h>
#include <memory/common.h>

#define BLOCK_SIZE 4096
#define BLOCKS_PER_BYTE 8

uint32_t* memory_map = 0;
uint32_t max_blocks = 0;
uint32_t used_blocks = 0;

void set_block(uint32_t bit)
{
	memory_map[bit / 32] |= (1 << (bit % 32));
}

void unset_block(uint32_t bit)
{
	memory_map[bit / 32] &= ~(1 << (bit % 32));
}

int32_t find_first_free_blocks(uint32_t num_blocks)
{
	if(num_blocks == 0) return -1;

	for(uint32_t i = 0; i < max_blocks / 32; i++)
	{
		if(memory_map[i] != 0xFFFFFFFF)
		{
			for(int32_t j = 0; j < 32; j++)
			{
				int32_t bit = 1 << j;
			
				if(!(memory_map[i] & bit))
				{
					for(uint32_t count = 0, free_blocks = 0; count < num_blocks; count++)
					{
						if((j + count > 31) && (i + 1 <= max_blocks / 32))
						{
							if(!(memory_map[i + 1] & (1 << ((j + count) - 32)))) free_blocks++;
						}
						else
						{
							if(!(memory_map[i] & (1 << (j + count)))) free_blocks++;
						}

						if(free_blocks == num_blocks) return i * 32 + j;
					}
				}
			}
		}
	}

	return -1;
}

void init_memory_manager(uint32_t start_address, uint32_t size)
{
	memory_map = (uint32_t*) start_address;
	max_blocks = size / BLOCK_SIZE;
	used_blocks = max_blocks;

	memset(memory_map, 0xFF, max_blocks / BLOCKS_PER_BYTE);
}

void init_memory_region(uint32_t base_address, uint32_t size)
{
	int32_t align = base_address / BLOCK_SIZE;
	int32_t num_blocks = size / BLOCK_SIZE;

	for(; num_blocks > 0; num_blocks--)
	{
		unset_block(align++);
		used_blocks--;
	}

	set_block(0);
}

void deinit_memory_region(uint32_t base_address, uint32_t size)
{
	int32_t align = base_address / BLOCK_SIZE;
	int32_t num_blocks = size / BLOCK_SIZE;

	for(; num_blocks > 0; num_blocks--)
	{
		set_block(align++);
		used_blocks++;
	}
}

uint32_t* alloc_blocks(uint32_t num_blocks)
{
	if((max_blocks - used_blocks) <= num_blocks) return 0;

	int32_t starting_block = find_first_free_blocks(num_blocks);
	if(starting_block == -1) return 0;

	for(uint32_t i = 0; i < num_blocks; i++) set_block(starting_block + i);

	used_blocks += num_blocks;

	uint32_t address = starting_block * BLOCK_SIZE;

	return (uint32_t*) address;
}

void free_blocks(uint32_t* address, uint32_t num_blocks)
{
	int32_t starting_block = (uint32_t) address / BLOCK_SIZE;

	for(uint32_t i = 0; i < num_blocks; i++) unset_block(starting_block + i);

	used_blocks -= num_blocks;
}
<<<<<<< HEAD
=======


























typedef struct MemoryChunk
{
	struct MemoryChunk* next;
    struct MemoryChunk* prev;
    bool allocated;
    size_t size;
} __attribute__((packed)) MemoryChunk;

MemoryChunk* first;

void heap_init(size_t start, size_t size)
{    
    if(size < sizeof(MemoryChunk))
    {
        first = 0;
    }
    else
    {
        first = (MemoryChunk*)start;
        
        first -> allocated = false;
        first -> prev = 0;
        first -> next = 0;
        first -> size = size - sizeof(MemoryChunk);
    }
}
        
void* malloc(size_t size)
{
    MemoryChunk *result = 0;
    
    for(MemoryChunk* chunk = first; chunk != 0 && result == 0; chunk = chunk->next)
        if(chunk->size > size && !chunk->allocated)
            result = chunk;
        
    if(result == 0)
        return 0;
    
    if(result->size >= size + sizeof(MemoryChunk) + 1)
    {
        MemoryChunk* temp = (MemoryChunk*)((size_t)result + sizeof(MemoryChunk) + size);
        
        temp->allocated = false;
        temp->size = result->size - size - sizeof(MemoryChunk);
        temp->prev = result;
        temp->next = result->next;
        if(temp->next != 0)
            temp->next->prev = temp;
        
        result->size = size;
        result->next = temp;
    }
    
    result->allocated = true;
    return (void*)(((size_t)result) + sizeof(MemoryChunk));
}

void free(void* ptr)
{
    MemoryChunk* chunk = (MemoryChunk*)((size_t)ptr - sizeof(MemoryChunk));
    
    chunk -> allocated = false;
    
    if(chunk->prev != 0 && !chunk->prev->allocated)
    {
        chunk->prev->next = chunk->next;
        chunk->prev->size += chunk->size + sizeof(MemoryChunk);
        if(chunk->next != 0)
            chunk->next->prev = chunk->prev;
        
        chunk = chunk->prev;
    }
    
    if(chunk->next != 0 && !chunk->next->allocated)
    {
        chunk->size += chunk->next->size + sizeof(MemoryChunk);
        chunk->next = chunk->next->next;
        if(chunk->next != 0)
            chunk->next->prev = chunk;
    }
    
}
>>>>>>> d183a15 (ELF32 Loader working)
