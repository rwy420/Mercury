#include <fs/vfs/vfs.h>
#include <fd.h>
#include <common/screen.h>
#include <vesa.h>
#include <memory/heap.h>
#include <fs/rootfs/rootfs.h>

extern FileDescriptor g_file_descriptors[];
VFSNode* root_vfs_node;

int vfs_init()
{
	root_vfs_node = kmalloc(sizeof(VFSNode));
	VFSMount* root_mount = rootfs_mount();
	root_vfs_node = &root_mount->root;

	printf("<VFS> VFS Initialized\n");

	return 0;
}

VFSNode* vfs_resolve(char* path)
{
	VFSNode* current = 0;

	char* start = path;

	if(*start == '/') start++;

	while(*start)
	{
		char* end = start;

		while(*end && *end != '/')
		{
			end++;
		}

		char saved = *end;
		*end = '\0';

		VFSNode* next = root_vfs_node->ops->lookup(current, start);
		if(next == 0) goto not_found;

		if(next->mounted) current = &next->mounted->root;
		else current = next;

		*end = saved;

		start = end;

		while(*start == '/') start++;
	}

	

	if(current == 0)
	{
not_found:
		printf_color("<VFS> Could not find '", COLOR_RED, COLOR_BLACK);
		printf_color(path, COLOR_RED, COLOR_BLACK);
		printf_color("'\n", COLOR_RED, COLOR_BLACK);
		return 0;
	}

	return current;
}

int vfs_mount()
{

}

uint32_t vfs_open(char* path)
{
	FileDescriptor* fd = create_fd();
	VFSNode* node = vfs_resolve(path);

	
	return fd->id;
}

int vfs_read(uint32_t fd_id, void* buffer, size_t length)
{

}

int vfs_write(uint32_t fd_id, void* buffer, size_t length)
{

}

int vfs_close(uint32_t fd_id)
{

}

int vfs_seek(uint32_t fd_id, uint32_t offset)
{

}
