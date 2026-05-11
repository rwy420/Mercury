#ifndef __MERCURY__FS__FS_H
#define __MERCURY__FS__FS_H

#include <common/types.h>

uint32_t fs_open(char* path);
int fs_read(uint32_t fd, void* buffer, size_t length);
int fs_write(uint32_t fd, void* buffer, size_t length);
int fs_close(uint32_t fd);
int fs_seek(uint32_t fd, uint32_t offset);

#endif
