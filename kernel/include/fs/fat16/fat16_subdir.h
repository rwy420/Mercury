#ifndef __MERCURY__FS__FAT16__FAT16_SUBDIR_H
#define __MERCURY__FS__FAT16__FAT16_SUBDIR_H

#include <fs/fat16/fat16_priv.h>

int ls_in_subdir(uint32_t* index, char* name, EntryHandle* handle);
int open_file_in_subdir(EntryHandle* handle, char* filename, char mode);
int open_directory_in_subdir(EntryHandle* handle, char* dirname);
uint32_t subdir_entry_size(EntryHandle* handle, char* filename);

#endif
