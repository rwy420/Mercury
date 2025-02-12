#ifndef __QS__FS__FAT16__FAT16_ROOTDIR_H
#define __QS__FS__FAT16__FAT16_ROOTDIR_H

#include <fs/fat16/fat16_priv.h>
#include <core/types.h>

int ls_in_root(uint32_t* index, char* filename);
int open_directory_in_root(EntryHandle* handle, char* dirname);

#endif
