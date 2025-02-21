#ifndef __MERCURY__FD_H
#define __MERCURY__FD_H

#include <hardware/interrupts.h>
#include <common/types.h>

typedef enum
{
	FD_FILE,
	FD_DL,
	FD_DEVICE
} FILE_DESCRIPTOR_TYPE;

typedef struct
{
	int index;
	FILE_DESCRIPTOR_TYPE type;
	void* object;
	int seek;
	void (*read)(void* buffer, size_t length);
	void (*write)(void* buffer, size_t length);
	void (*close)();
} FileDescriptor;

void fd_init();
FileDescriptor* create_fd();
void close_fd(int fd);

void syscall_read(CPUState* cpu);
void syscall_write(CPUState* cpu);
void syscall_seek(CPUState* cpu);

#endif
