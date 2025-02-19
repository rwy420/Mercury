#ifndef __MERCURY__FS__FAT16__FAT16_ROOTDIR_H
#define __MERCURY__FS__FAT16__FAT16_ROOTDIR_H

#include <fs/fat16/fat16_priv.h>
#include <common/types.h>

int ls_in_root(uint32_t* index, char* filename);
int open_file_in_root(EntryHandle* handle, char* filename, char mode);
uint32_t root_directory_entry_size(char* filename);
int open_directory_in_root(EntryHandle* handle, char* dirname);

#endif
