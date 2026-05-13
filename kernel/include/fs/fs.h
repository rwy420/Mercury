#ifndef __MERCURY__FS__FS_H
#define __MERCURY__FS__FS_H

#include <common/types.h>

uint32_t fs_open(char* path);
int fs_read(uint32_t fd_id, void* buffer, size_t length);
int fs_write(uint32_t fd_id, void* buffer, size_t length);
int fs_close(uint32_t fd_id);
int fs_seek(uint32_t fd_id, uint32_t offset);

#endif
