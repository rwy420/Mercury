#ifndef __STDIO_H
#define __STDIO_H

#include "./types.h"

typedef enum
{
    SEEK_SET, 
	SEEK_CUR,
	SEEK_END
} SEEK_WHENCE;

size_t read(int fd, const char* buffer, size_t length);
size_t write(int fd, const char* buffer, size_t length);
int open(const char* path, int flags/*TODO*/);
int close(int fd);
uint32_t lseek(int fd, uint32_t offset, int whence);

#endif
