#include "./stdio.h"
#include "./syscall.h"

void read(unsigned int fd, char* buffer, unsigned int length)
{
	syscall(0x03, fd, (unsigned int) buffer, length, 0, 0);
}

void write(unsigned int fd, char* buffer, unsigned int length)
{
	syscall(0x04, fd, (unsigned int) buffer, length, 0, 0);
}

int open(char* path, int flags, char mode)
{
	return syscall(0x05, (unsigned int) path, flags, mode, 0, 0);
}

void close(unsigned int fd)
{
	syscall(0x06, fd, 0, 0, 0, 0);
}

void lseek(unsigned int fd, unsigned int offset, unsigned int whence)
{
	syscall(0x13, fd, offset, whence, 0, 0);
} 
