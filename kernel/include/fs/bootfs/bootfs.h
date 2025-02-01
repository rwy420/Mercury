#ifndef __QS__FS__BOOTFS__BOOTFS_H
#define __QS__FS__BOOTFS__BOOTFS_H

#include <core/types.h>

typedef struct
{
	uint8_t name[22];
	uint8_t type[2];
	uint32_t start;
	uint32_t end;
} __attribute__((packed)) BootFS_Entry;

void read_files();

#endif
