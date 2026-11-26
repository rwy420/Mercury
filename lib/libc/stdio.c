#include "./stdio.h"
#include "./syscall.h"

size_t read(int fd, const char* buffer, size_t length)
{
	return syscall(0x03, fd, (uint32_t) buffer, length, 0, 0);
}

size_t write(int fd, const char* buffer, size_t length)
{
	return syscall(0x04, fd, (uint32_t) buffer, length, 0, 0);
}

int open(const char* path, int flags)
{
	return syscall(0x05, (uint32_t) path, flags, 0, 0, 0);
}

int close(int fd)
{
	return syscall(0x06, fd, 0, 0, 0, 0);
}

uint32_t lseek(int fd, uint32_t offset, int whence)
{
	return syscall(0x13, fd, offset, whence, 0, 0);
} 
