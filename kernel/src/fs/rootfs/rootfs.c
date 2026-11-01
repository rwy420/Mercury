#include "common/types.h"
#include <fs/vfs/vfs.h>
#include <fs/rootfs/rootfs.h>
#include <memory/heap.h>
#include <memory/common.h>
#include <common/screen.h>

VFSNodeOps rootfs_ops =
{
	.lookup = rootfs_lookup
};

FileSystem rootfs =
{
	.name = "RootFS",
	.ops = &rootfs_ops
};

VFSMount* rootfs_mount()
{
	RootFSNode* rootfs_root_dir = kmalloc(sizeof(RootFSNode));
	memset(rootfs_root_dir, 0, sizeof(RootFSNode));

	VFSMount* mount = kmalloc(sizeof(VFSMount));
	VFSNode* root_node = &mount->root;

	root_node->fs_object = rootfs_root_dir;
	root_node->ops = &rootfs_ops;
	root_node->fs_object = rootfs_root_dir;

	mount->fs = &rootfs;

	return mount;
}

void* rootfs_lookup(VFSNode* dir, char* name)
{
	RootFSNode* rootfs_root_dir = (RootFSNode*) dir->fs_object;

	for(int i = 0; i < 32; i++)
	{
		RootFSNode* current = rootfs_root_dir->children[i];

		//if(current->attributes != 0x0) printf("a");
	}

	return 0;
}

void* rootfs_create_dir(VFSNode* parent, char* name)
{
	RootFSNode* rootfs_root_dir = (RootFSNode*) parent->fs_object;

	for(int i = 0; i < ROOTFS_MAX_CHILDREN; i++)
	{
		RootFSNode* child = rootfs_root_dir->children[i];
		
		print_hex32((uint32_t) child);
	}

	return 0;
}
