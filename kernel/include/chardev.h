#ifndef __MERCURY__CHARDEV_H
#define __MERCURY__CHARDEV_H

#include <types.h>

typedef struct
{
	int (*read)(uint32_t dev, void* buffer, size_t size);
	int (*write)(uint32_t dev, void* buffer, size_t size);
} CharDevOps;

#endif
