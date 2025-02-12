#ifndef __QS__FS__FAT16__FAT16_SUBDIR_H
#define __QS__FS__FAT16__FAT16_SUBDIR_H

#include <fs/fat16/fat16_priv.h>

int ls_in_subdir(uint32_t* index, char* name, EntryHandle* handle);
int open_directory_in_subdir(EntryHandle* handle, char* dirname);

#endif
