#include <fs/devfs/devfs.h>
#include <fs/vfs/vfs.h>

VFSNodeOps devfs_ops =
{
	.read_dir = devfs_read_dir 
};

VFSNode** devfs_read_dir(VFSNode* node)
{
	return 0;
}
