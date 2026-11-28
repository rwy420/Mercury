#ifndef __MERCURY__BLKDEV_H
#define __MERCURY__BLKDEV_H

#include <types.h>

typedef struct
{
	int (*read_blocks)(uint32_t dev, uint32_t lba, void* buffer, size_t size);
	int (*write_blocks)(uint32_t dev, uint32_t lba, void* buffer, size_t size);
} BlkDevOps;

typedef struct BlkDev
{
	uint32_t block_size;
	uint64_t block_count;
	uint32_t flags;
	BlkDevOps* ops;
	struct BlkDev* parent;
	uint64_t start_lba;
	void* data;
} BlkDev;

#endif
