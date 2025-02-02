#ifndef __QS__FS__MERCURYFS__MERCURYFS_H
#define __QS__FS__MERCURYFS__MERCURYFS_H

#include <core/types.h>

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

Directory* get_root();
void write_block(int block_idx, uint8_t* data);
void write_inode(Inode* inode);
Inode* load_inode(int inode_idx);
Block* load_block(int block_idx);
void cache_directory(Directory* directory);
Inode* get_inode(int inode_idx);
int count_entries(Directory* directory);
int file_used_blocks(Inode* file);
int find_free_dir_entry(Directory* directory);
int find_free_inode();
int find_free_block();
Directory* get_inode_directory(Inode* directory);
Inode* get_inode_name(Directory* directory, char* name);
void mark_block_used(int block_idx);
int create_directory(char* name, Directory* parent);
void list_directory(Directory* directory);
int create_file(char* name, uint8_t type, Directory* parent);
void prealloc_file(Inode* file, int blocks);
void write_file(int directory_entry_idx, Directory* parent, char* data, int data_len);
void write_root();
void write_cached_inodes();
void write_header();
Directory* get_dir_from_name(char* name, Directory* parent);
void mercuryfs_init();

#endif
