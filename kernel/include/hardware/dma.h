#ifndef __MERCURY__HARDWARE__DMA_H
#define __MERCURY__HARDWARE__DMA_H

#include <common/types.h>

typedef struct
{
	uint32_t phys;
	size_t size;
} DMARegion;

DMARegion* dma_create(size_t size);
void dma_free(DMARegion* region);

#endif
