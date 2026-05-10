#ifndef __MERCURY__FS__FAT32__FAT32_DIR_H
#define __MERCURY__FS__FAT32__FAT32_DIR_H

#include <fs/fat/fat.h>

FATDirectoryEntry* fat_path_to_dir_entry(uint32_t root_cluster, char* path);

#endif
