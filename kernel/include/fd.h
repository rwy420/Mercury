#ifndef __MERCURY__FD_H
#define __MERCURY__FD_H

#define MAX

#include <common/types.h>

typedef enum
{
	FD_FILE,
	FD_DEVICE
} FILE_DESCRIPTOR_TYPE;

typedef struct
{
	int index;
	FILE_DESCRIPTOR_TYPE type;
	void* object;
	int seek;
	int (*read)(void* buffer, size_t length);
	int (*write)(void* buffer, size_t length);
	int *(close)();
} FileDescriptor;

void fd_init();
FileDescriptor* create_fd();
FileDescriptor* get_fd(int fd);
void close_fd(int fd);

#endif
