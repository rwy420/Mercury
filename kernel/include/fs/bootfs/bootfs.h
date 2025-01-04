/*
 * Idea:
 * - Really basic FS just containing the kernel and modules for the kernel
 * - First 8 Sectors consist of the file table
 * - File table entry: char[22] name, uint8 type [kernel, module..], uint32 start, uint32 end uint8[1] filler -> 32 Bytes
 * - With each entry being 32 bytes in size, we can have 128 modules + kernel
 * - Kernel must be the first entry
*/
#ifndef __MERCURY__FS__BOOTFS__BOOTFS_H
#define __MERCURY__FS__BOOTFS__BOOTFS_H

#include <core/types.h>

typedef struct
{
	uint8_t name[22];
	uint8_t type;
	uint32_t start;
	uint32_t end;
	uint8_t null;
} __attribute__((packed)) BootFS_Entry;

void read_files();

#endif
