#ifndef __MERCURY__DEV_H
#define __MERCURY__DEV_H

#define MAX_DEV_NUM 0x10

#include <types.h>

typedef enum
{
	DEV_CHAR,
	DEV_BLK
} DEVICE_TYPE;

typedef struct
{
	char name[16];
	uint32_t attributes;
	void* ops;
} Device;

int device_register(Device* device);

#endif
