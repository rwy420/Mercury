#ifndef __MERCURY__FD_H
#define __MERCURY__FD_H

#include <hardware/interrupts.h>
#include <common/types.h>

typedef enum
{
	FD_FILE,
	FD_DEVICE
} FILE_DESCRIPTOR_TYPE;

typedef enum
{
	SEEK_SET,
	SEEK_CUR,
	SEEK_END
} SEEK_WHENCE;

typedef struct
{
	int index;
	FILE_DESCRIPTOR_TYPE type;
	void* object;
	int seek;
	int (*read)(void* buffer, size_t length);
	int (*write)(void* buffer, size_t length);
	int (*close)();
} FileDescriptor;

void fd_init();
FileDescriptor* create_fd();
void close_fd(int fd);

int syscall_read(CPUState* cpu);
int syscall_write(CPUState* cpu);
int syscall_open(CPUState* cpu);
int syscall_close(CPUState* cpu);
int syscall_lseek(CPUState* cpu);

#endif
