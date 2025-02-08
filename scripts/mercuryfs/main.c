#include <complex.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "./structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SECTOR_SIZE 512

int read_sector(int sector_number, void *buffer) {
    FILE *file = fopen("fs.bin", "rb");
    if (!file) {
        perror("Error opening file for reading");
        return -1;
    }

    if (fseek(file, sector_number * SECTOR_SIZE, SEEK_SET) != 0) {
        perror("Error seeking file position");
        fclose(file);
        return -1;
    }

    if (fread(buffer, SECTOR_SIZE, 1, file) != 1) {
        perror("Error reading sector");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

int write_sector(int sector_number, const void *buffer) {
    FILE *file = fopen("fs.bin", "r+b");
    if (!file) {
        file = fopen("fs.bin", "wb");
        if (!file) {
            perror("Error opening file for writing");
            return -1;
        }
    }

    if (fseek(file, sector_number * SECTOR_SIZE, SEEK_SET) != 0) {
        perror("Error seeking file position");
        fclose(file);
        return -1;
    }

    if (fwrite(buffer, SECTOR_SIZE, 1, file) != 1) {
        perror("Error writing sector");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

int block_offset = 512;
int inode_offset = 512 + (sizeof(Block) * MAX_BLOCKS);
int root_offset = 512 + ((sizeof(Block) * MAX_BLOCKS) + (sizeof(Inode) * MAX_INODES));

fsHeader* header;
Cache* cache;
int cached_inodes_count = 1;
Directory* root;

void write_block(int block_idx, uint8_t* data)
{
	int sector = (block_offset / 512) + ((block_idx * sizeof(Block) / 512));

	for(int i = 0; i < sizeof(Block) / 512; i++)
	{
		write_sector(sector + i, (uint8_t*)(data + (i * 512)));
	}
}

void write_inode(Inode* inode)
{
	int bytes = inode_offset + (sizeof(Inode) * inode->real_idx);
	int sector = bytes / 512;
	int sector_first_byte = bytes - (sector * 512);

	uint8_t* write_buffer = malloc(3 * 512);

	for(int i = 0; i < 3; i++)
	{
		read_sector(sector + i, (uint8_t*) write_buffer + (i * 512));
	}

	memcpy((uint8_t*) write_buffer + sector_first_byte, (uint8_t*) inode, sizeof(Inode));

	for(int i = 0; i < 3; i++)
	{
		write_sector(sector + i, write_buffer + (i * 512));
	}

	free(write_buffer);
}

Inode* load_inode(int inode_idx)
{
	int byte = inode_offset + (sizeof(Inode) * inode_idx);
	int sector = byte / 512;
	int sector_first_byte = byte - (sector * 512);
	Inode* inode = malloc(sizeof(Inode));

	uint8_t* read_buffer;

	read_buffer = malloc(3 * 512);

	for(int idx = 0; idx < 3; idx++)
	{
		read_sector(sector + idx, (uint8_t*) read_buffer + (idx * 512));
	}

	memcpy(inode, (uint8_t*) read_buffer + sector_first_byte, sizeof(Inode));
	free(read_buffer);

	return inode;
}

Block* load_block(int block_idx)
{
	int byte = block_offset + (sizeof(Block) * block_idx);
	int first_sector = byte / 512;

	Block* block = malloc(sizeof(Block));
	uint8_t* read_buffer = malloc(BLOCK_SIZE);

	for(int i = 0; i < sizeof(Block) / 512; i++)
	{
		read_sector(first_sector + i, (uint8_t*) read_buffer + (i * 512));
	}

	memcpy(block, read_buffer, sizeof(Block));
	free(read_buffer);

	return block;
}

void cache_directory(Directory* directory)
{
	if(cache) free(cache);
	DirEntry* entries = (DirEntry*) load_block(directory->self.block_pointers[0]);

	cache = malloc(sizeof(Cache));

	cache->cached_directory = directory;

	for(int i = 1; i < MAX_DIR_ENTRIES; i++)
	{
		//DirEntry* entry = &(directory->entries[i]);
		DirEntry* entry = &entries[i];
		CachedInode cached_inode;

		if(entry->inode_number == 0x00) break;

		cached_inode.real_idx = entry->inode_number;
		cached_inode.data = load_inode(cached_inode.real_idx);

		cache->inodes[i] = cached_inode;
		cached_inodes_count = i + 1;
	}
}

Inode* get_inode(int inode_idx)
{
	for(int i = 1; i < MAX_DIR_ENTRIES + 1; i++)
	{
		if(cache->inodes[i].real_idx == 0x00)
		{
			break;
		}

		if(cache->inodes[i].real_idx == inode_idx)
		{
			return cache->inodes[i].data;
		}
	}

	return load_inode(inode_idx);
}

int count_entries(Directory* directory)
{
	int count = 0;

	/*for(int i = 1; i < MAX_INODES + 1; i++)
	{
		if(directory->entries[i].inode_number == 0) break;

		count++;
	}*/

	return count;
}


int file_used_blocks(Inode* file)
{
	/*int used = 0;

	for(int i = 0; i < MAX_FILE_BLOCKS; i++)
	{
		if(file->block_pointers[i] == 0) break;

		used++;
	}

	if(file->extent_block != 0)
	{
	}*/

	return file->blocks;
}

int find_free_dir_entry(Directory* directory)
{
	//DirEntry* entries = directory->entries;
	DirEntry* entries = (DirEntry*) load_block(directory->self.block_pointers[0]);
	
	for(int i = 1; i < MAX_DIR_ENTRIES; i++)
	{
		if(entries[i].inode_number == 0) return i;
	}

	return -1;
}

int find_free_inode()
{
	return header->next_inode++;
}

int find_free_block()
{
	return header->next_block++;
}

Directory* get_inode_directory(Inode* directory)
{
	//FIXME
	//return (Directory*) &(fs->blocks[directory->block_pointers[0]]);
	return (Directory*) load_block(directory->block_pointers[0]);
}

Inode* get_inode_name(Directory* directory, char* name)
{
	//DirEntry* entries = (DirEntry*) &directory->entries;
	DirEntry* entries = (DirEntry*) load_block(directory->self.block_pointers[0]);
	
	for(int i = 1; i < MAX_DIR_ENTRIES; i++)
	{
		DirEntry* entry = &entries[i];
		Inode* inode = get_inode(entry->inode_number);

		if(entry->inode_number == 0x00) break;

		if(!strcmp(name, inode->name)) return inode;
	}

	return 0;
}

void mark_block_used(int block_idx)
{
	int bytes = block_offset + (block_idx * BLOCK_SIZE);
	int sector = bytes / 512;

	uint8_t* write_buffer = malloc(512);
	write_buffer[0] = 1;

	write_sector(sector, write_buffer);
}

void extend_file(Inode* file)
{
	int block_pointer = find_free_block();
	file->extent_block = block_pointer;

	mark_block_used(block_pointer);
	write_inode(file);
}

void alloc_extend(Inode* file, uint32_t blocks)
{
	int start = 0;
	Block* block_buffer = load_block(file->extent_block);
	memset(block_buffer, 0x0, sizeof(Block));
	Extent* extends = (Extent*) block_buffer->data;

	for(int i = 0; i < (BLOCK_SIZE - 1) / sizeof(Extent); i++)
	{
		if(extends[i].start_block == 0)
		{
			start = i;
			break;
		}
	}

	//FIXME
	//for(int i = 0; i < blocks; i++)
	//{
	//}
	
	uint32_t block = find_free_block();
	extends[start].start_block = block;
	extends[start].size = blocks;

	block_buffer->used = 0x01;
	mark_block_used(file->extent_block);
	write_block(file->extent_block, (uint8_t*) block_buffer);

	for(uint32_t b = 0; b < blocks; b++)
	{
		mark_block_used(block + b);
	}

	file->blocks += blocks;
	write_inode(file);
}

int create_directory(char* name, Directory* parent)
{
	int directory_idx = find_free_dir_entry(parent);
	int inode_idx = find_free_inode();
	int block_idx = find_free_block();

	//DirEntry* entry = &(parent->entries[directory_idx]);
	DirEntry* entries = (DirEntry*) load_block(parent->self.block_pointers[0]);
	DirEntry* entry = &entries[directory_idx];

	//Inode* inode = &(fs->inodes[inode_idx]);
	Inode* inode = get_inode(inode_idx);
	
	entry->inode_number = inode_idx;
	strncpy(entry->name, name, 14);

	//FIXME
	//fs->blocks[block_idx].used = 1;
	Block* temp = load_block(block_idx);
	temp->used = 1;
	write_block(block_idx, (uint8_t*) temp);
	free(temp);

	strncpy(inode->name, name, 14);
	inode->block_pointers[0] = block_idx;
	inode->permissions = 1;
	inode->type = 0xFF;
	inode->real_idx = inode_idx;

	cache->inodes[cached_inodes_count].real_idx = inode_idx;
	cache->inodes[cached_inodes_count].data = inode;
	cached_inodes_count++;

	entries[directory_idx] = *entry;
	write_block(parent->self.block_pointers[0], (uint8_t*) entries);
	write_inode(inode);

	printf("Directory '%s' created at %d:%d\n", entry->name, directory_idx, inode_idx);

	return directory_idx;
}

void list_directory(Directory* directory)
{
	printf("Contents of %s:\n", directory->self.name);
	//DirEntry* entries = (DirEntry*) &directory->entries;
	DirEntry* entries = (DirEntry*) load_block(directory->self.block_pointers[0]);
	
	for(int i = 1; i < MAX_DIR_ENTRIES; i++)
	{
		DirEntry* entry = &entries[i];
		Inode* inode = get_inode(entry->inode_number);

		if(entry->inode_number == 0x00) break;

		printf("> %s", inode->name);
		if(inode->type == 0xFF) printf("/");
		printf("\n");
	}
}

int create_file(char* name, uint8_t type, Directory* parent)
{
	int directory_entry_idx = find_free_dir_entry(parent);
	int inode_idx = find_free_inode();
	int root_block_idx = find_free_block();

	//DirEntry* entry = &(parent->entries[directory_entry_idx]);
	DirEntry* entries = (DirEntry*) load_block(parent->self.block_pointers[0]);
	DirEntry* entry = &entries[directory_entry_idx];

	Inode* inode = get_inode(inode_idx);
	Block* root_block = load_block(root_block_idx);

	
	entry->inode_number = inode_idx;
	
	strncpy(entry->name, name, MAX_FILENAME_LEN);

	strncpy(inode->name, name, MAX_FILENAME_LEN);
	inode->access_time = 123;
	inode->creation_time = 123;
	inode->modification_time = 123;
	inode->permissions = 1;
	inode->size = sizeof(Block);
	inode->block_pointers[0] = root_block_idx;
	inode->type = type;
	inode->real_idx = inode_idx;
	inode->blocks = 1;

	root_block->used = 1;
	write_block(root_block_idx, (uint8_t*) root_block);

	printf("%d;%d\n", cache->cached_directory->self.block_pointers[0], parent->self.block_pointers[0]);

	if(cache->cached_directory->self.block_pointers[0] == parent->self.block_pointers[0])
	{
		cache->inodes[cached_inodes_count].real_idx = inode_idx;
		cache->inodes[cached_inodes_count].data = inode;
		cached_inodes_count++;
	}

	entries[directory_entry_idx] = *entry;
	write_block(parent->self.block_pointers[0], (uint8_t*) entries);
	write_inode(inode);

	printf("File '%s' created at %d:%d:%d\n", entry->name, directory_entry_idx, inode_idx, root_block_idx);	
	return directory_entry_idx;
}

void prealloc_file(Inode* file, int blocks)
{
	for(int i = 0; i < blocks; i++)
	{
		int block_id = find_free_block();
		mark_block_used(block_id);
		file->block_pointers[i] = block_id;
	}

	file->blocks += blocks;

	write_inode(file);
}

void write_file(int directory_entry_idx, Directory* parent, char* data, int data_len)
{
	// FIXME
	/*
	Inode* inode = &(fs->inodes[parent->entries[directory_entry_idx].inode_number]);
	int used_blocks = file_used_blocks(directory_entry_idx, parent);
	int required_blocks = (data_len + BLOCK_SIZE) / BLOCK_SIZE;

	for(int i = 0; i < required_blocks - used_blocks; i++)
	{
		int extra_block = find_free_block();
		fs->blocks[extra_block].used = 1;
		inode->block_pointers[used_blocks + i] = extra_block;
	}

	printf("%d\n", required_blocks);

	int bytes = 0;

	for(int i = 0; i < required_blocks; i++)
	{
		int block_idx = inode->block_pointers[i];
		printf("Writing to block %d\n", block_idx);

		int first_sector = block_idx * (BLOCK_SIZE / 512);
		for(int i = first_sector; i < first_sector + (BLOCK_SIZE / 512); i++)
		{
			int write_len = 512;
			if(bytes + 512 > data_len)
			{
				write_len = data_len - bytes;
			}

			uint8_t* buffer = malloc(512);
			memcpy(buffer, data + ((i - first_sector) * 512) , write_len);
			if(write_len < 512)
			{
				memset(buffer + write_len, 0x0, 512 - write_len);
			}
			write_sector(i, buffer);
			free(buffer);
			bytes += 512;
		}
	}


	printf("Finished\n");
	*/
}

void write_root()
{
	for(int i = 0; i < ((sizeof(Directory) + 512) / 512); i++)
	{
		int sector = (root_offset / 512) + i;
		uint8_t* write_buffer = malloc(512);
		memset(write_buffer, 0x0, 512);
		memcpy(write_buffer, ((uint8_t*) root) + (i * 512), 512);

		write_sector(sector, write_buffer);
		free(write_buffer);
	}	
}

void write_cached_inodes()
{
	for(int i = 1; i < MAX_DIR_ENTRIES; i++)
	{
		CachedInode* c_inode = &(cache->inodes[i]);
		if(c_inode->real_idx == 0) break;

		write_inode(c_inode->data);
	}
}

void write_header()
{
	uint8_t* write_buffer = malloc(512);
	memset(write_buffer, 0x0, 512);
	memcpy(write_buffer, ((uint8_t*) header), 512);

	write_sector(0, write_buffer);
	free(write_buffer);
}

void mk_root_fs()
{
	header = malloc(sizeof(fsHeader));
	header->next_block = 2;
	header->next_inode = 1;

	root = malloc(sizeof(Directory));
	uint8_t* name_buffer = "root";
	memcpy(root->self.name, name_buffer, 4);
	root->self.block_pointers[0] = 1;
	root->self.type = 0xFF;

	write_root();
	write_header();
}

Directory* get_dir_from_name(char* name, Directory* parent)
{

	Inode* dir = get_inode_name(parent, name);
	DirEntry* dir_entries = (DirEntry*) load_block(dir->block_pointers[0]);

	Directory* dir_d = malloc(sizeof(Directory));
	dir_d->self.block_pointers[0] = dir->block_pointers[0];
	memcpy(dir_d->self.name, dir->name, MAX_FILENAME_LEN);

	return dir_d;
}


void write_file_bin(char* file_path, char* name, Directory* parent)
{
	//Directory* sbin = get_dir_from_name("sbin", root);
	//Inode* os = get_inode_name(sbin, "mercury");
	
	Inode* inode = get_inode_name(parent, name);

	int blocks = file_used_blocks(inode);
	printf("'%s' preallocated size: %d KB\n", inode->name, (blocks * BLOCK_SIZE) / 1024);

	FILE* file;
	uint8_t* file_buffer;
	file = fopen(file_path, "rb");
	if(file == NULL)
	{
		printf("Error opening %s\n", file_path);
		return;
	}

	fseek(file, 0, SEEK_END);
	int file_size = ftell(file);

	if((blocks * BLOCK_SIZE) < file_size)
	{
		printf("Allocated file not large enough");
		return;
	}

	rewind(file);

	int file_size_blocks = (file_size + BLOCK_SIZE + BLOCK_SIZE) / BLOCK_SIZE;
	file_buffer = malloc(file_size_blocks * BLOCK_SIZE);
	fread(file_buffer, 1, file_size, file);

	Block* v_blocks = malloc(sizeof(Block) * blocks);

	for(int i = 0; i < file_size_blocks; i++)
	{
		v_blocks[i].used = 0x01;
		memcpy(&v_blocks[i].data, (uint8_t*) file_buffer + ((BLOCK_SIZE - 1) * i), BLOCK_SIZE - 1);

		write_block(inode->block_pointers[i], (uint8_t*) &v_blocks[i]);
	}

	free(file_buffer);
	free(v_blocks);
}

void write_root_fs()
{
	create_directory("drvr", root);
	Directory* dev_d = get_dir_from_name("drvr", root);
	create_directory("mnt", root);
	Directory* mnt_d = get_dir_from_name("mnt", root);
	create_directory("var", root);
	Directory* var_d = get_dir_from_name("var", root);
	create_directory("bin", root);
	Directory* bin_d = get_dir_from_name("bin", root);
	create_directory("sbin", root);
	Directory* sbin_d = get_dir_from_name("sbin", root);

	create_file("config", 0x10, root);

	create_file("mercury", 0x01, sbin_d);
	Inode* mercury_f = get_inode_name(sbin_d, "mercury");
	prealloc_file(mercury_f, MAX_FILE_BLOCKS);
	extend_file(mercury_f);
	alloc_extend(mercury_f, 128);

	printf("%x\n", block_offset + (mercury_f->extent_block * BLOCK_SIZE));

	write_root();
	write_header();

	list_directory(root);
}

void cmd_ls(char* name)
{
	Directory* dir = get_dir_from_name(name, root);

	list_directory(dir);
}

int main(int argc, char* argv[])
{
	if(argc == 2 && !strcmp(argv[1], "help"))
	{
		printf("MercuryFS Toolkit: \nmkroot - Make rootFS\nwroot - Write rootFS\nwos [file] - Write bin to mercuryOS bin file\n");
		printf("fsmaster ls [dir] - List directory, leave empty to list root\n");
		printf("fsmaster mkfile [name] [parentname] -  Make file\n");
		printf("fsmaster cpfile [path] [name] [parentname] - Copy file into mercuryFS\n");
		return 0;
	}

	if(argc == 2 && !strcmp(argv[1], "mkroot"))
	{
		printf("Making rootFS\n");
		mk_root_fs();
		return 0;
	}

	header = malloc(512);
	read_sector(0, header);

	uint8_t* root_buffer = malloc(sizeof(Directory) + 512);
	for(int i = 0; i < (sizeof(Directory) + 512) / 512; i++)
	{
		read_sector((root_offset / 512) + i, root_buffer + i * 512);
	}
	
	root = malloc(sizeof(Directory));
	memcpy(root, root_buffer, sizeof(Directory));
	
	printf("%s\n", root->self.name);

	cache_directory(root);

	if(argc == 2 && !strcmp(argv[1], "wroot"))
	{
		printf("Writing rootFS\n");
		write_root_fs();
		return 0;
	}

	if(argc == 3 && !strcmp(argv[1], "wos"))
	{
		printf("Writing OS bin\n");
		//write_os(argv[2]);
		write_file_bin(argv[2], "mercury", get_dir_from_name("sbin", root));
		return 0;
	}

	if(argc >= 3 && !strcmp(argv[1], "fsmaster"))
	{

		if(!strcmp(argv[2], "ls"))
		{
			if(argc == 3)
			{
				list_directory(root);
				return 0;
			}

			cmd_ls(argv[3]);
			return 0;
		}

		if(!strcmp(argv[2], "mkfile"))
		{
			char* name = argv[3];
			Directory* parent = get_dir_from_name(argv[4], root);
			create_file(name, 0x01, parent);
		}

		if(!strcmp(argv[2], "cpfile"))
		{
			char* path = argv[3];
			char* name = argv[4];
			char* parent_name = argv[5];

			Directory* parent = get_dir_from_name(parent_name, root);
			create_file(name, 0x01, parent);
			prealloc_file(get_inode_name(parent, name), MAX_FILE_BLOCKS);

			write_file_bin(path, name, parent);
		}
	}

	list_directory(root);
	
	write_header();
	write_root();
}
