#ifndef __MERCURY__FS__VFS__VFS_H
#define __MERCURY__FS__VFS__VFS_H

#include <common/types.h>

struct VFSNodeOps;
struct VFSMount;

typedef struct
{
	void* fs_object;
	struct VFSNodeOps* ops;
	struct VFSMount* mounted;
	uint32_t type;
} VFSNode;

typedef struct VFSNodeOps
{
	void* (*lookup)(VFSNode* dir, char* name);
	int (*open)(VFSNode* node);
	int (*read)(VFSNode* node, void* buffer, size_t size, size_t offset);
	int (*write)(VFSNode* node, void* buffer, size_t size, size_t offset);
	int (*close)(VFSNode* node);
} VFSNodeOps;

typedef struct
{
	char* name;
	VFSNodeOps* ops;
} FileSystem;

typedef struct VFSMount
{
	FileSystem* fs;
	VFSNode root;
} VFSMount;

int vfs_init();
VFSNode* vfs_resolve(char* path);
int vfs_mount();

uint32_t vfs_open(char* path);
int vfs_read(uint32_t fd_id, void* buffer, size_t length);
int vfs_write(uint32_t fd_id, void* buffer, size_t length);
int vfs_close(uint32_t fd_id);
int vfs_seek(uint32_t fd_id, uint32_t offset);

#endif
