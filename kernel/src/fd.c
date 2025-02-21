#include <fd.h>
#include <memory/common.h>
#include <common/screen.h>

FileDescriptor g_file_descriptors[0xFF];
int fd_count;

FileDescriptor* stdin;
FileDescriptor* stdout;

void fd_init()
{
	fd_count = 0;

	stdin = &g_file_descriptors[0];
	stdout = &g_file_descriptors[1];

	stdin->index = 0;
	stdout->index = 1;

	stdout->write = syscall_printf;
}

FileDescriptor* create_fd()
{
	for(int i = 0; i < 0xFF; i++)
	{
		if(g_file_descriptors[i].index == 0x100) 
		{
			g_file_descriptors[i].index = i;
			return &g_file_descriptors[i];
		}
	}

	return NULL_PTR;
}

void close_fd(int fd)
{
	memset(&g_file_descriptors[fd], 0, sizeof(FileDescriptor));
	g_file_descriptors[fd].index = 0x100;
}

void syscall_read(CPUState* cpu)
{

}

void syscall_write(CPUState* cpu)
{
	uint32_t fd_idx = cpu->ebx;
	char* buffer = (char*) cpu->ecx;
	size_t length = cpu->edx;

	FileDescriptor* fd = &g_file_descriptors[fd_idx];
	print_hex32((uint32_t) fd->write);
	if(fd->write != NULL_PTR) 
	{
		fd->write(buffer, length);
		return;
	}

	printf("Unknow FD ");
	print_hex32(fd_idx);
	printf("\n");
}

void syscall_seek(CPUState* cpu)
{

}
