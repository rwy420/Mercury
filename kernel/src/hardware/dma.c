#include <hardware/dma.h>
#include <memory/heap.h>
#include <memory/frames.h>
#include <memory/paging.h>

#include <common/screen.h>

extern PageDirectory* g_kernel_pd;

DMARegion* dma_create(size_t size)
{
	DMARegion* result = kmalloc(sizeof(DMARegion));

	uint32_t aligned_size = ((size + 0xFFF) & ~0xFFF) / FRAME_SIZE;
	void* frames = alloc_frames(aligned_size);

	if(!frames) return NULL_PTR;

	for(int i = 0; i < aligned_size; i++)
	{
		void* address = (void*) (((uint32_t) frames) + i * FRAME_SIZE);
		map_page_pd_flags(g_kernel_pd, address, address, PDE_PRESENT | PDE_RW | PDE_CACHE_DISABLE);
	}

	result->phys = (uint32_t) frames;
	result->size = aligned_size * FRAME_SIZE;

	return result;
}

void dma_free(DMARegion* region)
{
	free_frames((void*) region->phys, region->size / FRAME_SIZE);
	kfree(region);
}
