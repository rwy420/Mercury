#ifndef __MERCURY__BLKDEV_H
#define __MERCURY__BLKDEV_H

#include <types.h>

struct BlkDevOps;

typedef struct BlkDev
{
	uint32_t block_size;
	uint64_t block_count;
	uint32_t flags;
	struct BlkDev* parent;
	struct BlkDevOps* ops;
	uint64_t start_lba;
	void* data;
} BlkDev;

typedef struct BlkDevOps
{
	int (*read_blocks)(BlkDev* dev, uint32_t lba, void* buffer, size_t size);
	int (*write_blocks)(BlkDev* dev, uint32_t lba, void* buffer, size_t size);
	int (*flush)(BlkDev* dev);
} BlkDevOps;

#endif
