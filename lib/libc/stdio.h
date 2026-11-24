#ifndef __STDIO_H
#define __STDIO_H

typedef enum
{
    SEEK_SET, 
	SEEK_CUR,
	SEEK_END
} SEEK_WHENCE;

void read(unsigned int fd, char* buffer, unsigned int length);
void write(unsigned int fd, char* buffer, unsigned int length);
int open(char* path, int flags, char mode);
void close(unsigned int fd);
void lseek(unsigned int fd, unsigned int offset, unsigned int whence);

#endif
