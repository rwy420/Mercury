#ifndef __MERCURY__BLKDEV_H
#define __MERCURY__BLKDEV_H

#include <types.h>

typedef struct
{
	int (*read_blocks)(uint32_t dev, uint32_t lba, void* buffer, size_t size);
	int (*write_blocks)(uint32_t dev, uint32_t lba, void* buffer, size_t size);
} BlkDevOps;

#endif
