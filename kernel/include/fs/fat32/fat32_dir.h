#ifndef __MERCURY__FS__FAT32__FAT32_DIR_H
#define __MERCURY__FS__FAT32__FAT32_DIR_H

#include <fs/fat/fat.h>
#include <fs/vfs/vfs.h>

void* fat32_lookup(VFSNode* dir, char* name);

#endif
