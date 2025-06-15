#include <memory/paging.h>
#include <memory/heap.h>
#include <common/screen.h>
#include <memory/common.h>

MemoryChunk* first;

uint32_t heap_init(size_t start, size_t size)
{
    if(size < sizeof(MemoryChunk))
    {
        first = 0;
    }
    else
    {
        first = (MemoryChunk*) start;
        
        first -> allocated = false;
        first -> prev = 0;
        first -> next = 0;
        first -> size = size - sizeof(MemoryChunk);
    }

	uint32_t max = 0;
	for(uint32_t address = 0xC0400000; address < start + size - 0x200000; address += PAGE_SIZE)
	{
		max = address + 0x1000;

		//TODO: make sure this works
		uint32_t addr = address - 0xC0000000;
		//map_page((void*) address, (void*) addr);
	}

	return max;
}
        
void* kmalloc(size_t size)
{
    MemoryChunk *result = 0;
    
    for(MemoryChunk* chunk = first; chunk != 0 && result == 0; chunk = chunk->next)
        if(chunk->size > size && !chunk->allocated)
            result = chunk;
        
    if(result == 0)
        return 0;
    
    if(result->size >= size + sizeof(MemoryChunk) + 1)
    {
        MemoryChunk* temp = (MemoryChunk*)((size_t) result + sizeof(MemoryChunk) + size);
        
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

void kfree(void* ptr)
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

void* kmalloc_aligned(size_t alignment, size_t size)
{
	void* pointer = kmalloc(size + alignment - 1 + sizeof(void*));
	void** aligned_pointer = (void**)((void*) pointer + alignment - 1 + sizeof(void*));
	aligned_pointer[-1] = pointer;

	return (void*)(((size_t) aligned_pointer) & ~(alignment - 1));
}

void kfree_aligned(void* pointer)
{
	void* original_pointer = ((void**) pointer)[-1];
	kfree(original_pointer);
}
