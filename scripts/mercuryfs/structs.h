#include <stdint.h>

#define BLOCK_SIZE 4096
#define MAX_FILENAME_LEN 32
#define MAX_FILE_BLOCKS 128
#define MAX_DIR_ENTRIES 128
#define MAX_INODES 1024
#define MAX_BLOCKS (128 * 12)

typedef struct
{
	uint8_t used;
	uint8_t data[BLOCK_SIZE - 1];
} __attribute__((packed)) Block;

typedef struct
{
	uint32_t size;
	uint32_t block_pointers[MAX_FILE_BLOCKS];
	uint32_t extent_block;
	uint32_t blocks;
	uint32_t permissions;
	uint32_t creation_time;
	uint32_t modification_time;
	uint32_t access_time;
	char name[MAX_FILENAME_LEN];
	uint8_t type;
	uint32_t real_idx;
} __attribute__((packed)) Inode;

typedef struct
{
	uint32_t start_block;
	uint32_t size;
} __attribute__((packed)) Extent;

typedef struct
{
	char name[MAX_FILENAME_LEN];
	uint32_t inode_number;
}  __attribute__((packed)) DirEntry;

typedef struct
{
	//DirEntry entries[MAX_DIR_ENTRIES];
	Inode self;
}  __attribute__((packed)) Directory;

typedef struct
{
	Inode inode;
} File;

typedef struct
{
	uint32_t next_inode;
	uint32_t next_block;
	uint8_t empty[512 - 12];
	uint8_t magic[4];
}   __attribute__((packed)) fsHeader;

typedef struct
{
	fsHeader header;
	Block blocks[MAX_BLOCKS];
	Inode inodes[MAX_INODES];
	Directory root;
}   __attribute__((packed)) FileSystem;

typedef struct
{
	Inode* data;
	int real_idx;
}   __attribute__((packed)) CachedInode;

typedef struct
{
	CachedInode inodes[MAX_DIR_ENTRIES];
	Directory* cached_directory;
}   __attribute__((packed)) Cache;
