#include <fs/mercuryfs/mercuryfs.h>
#include <driver/ata/ata.h>
#include <memory/common.h>
#include <memory/mem_manager.h>
#include <core/screen.h>

void read_sector(uint32_t sector, void* buffer)
{
	read28(100 + sector, buffer, 512);
}

void write_sector(uint32_t sector, void* data)
{
	write28(100 + sector, data, 512);
}

uint32_t block_offset;
uint32_t inode_offset;
uint32_t root_offset;

fsHeader* header;
Cache* cache;
int cached_inodes_count;
Directory* root;

Directory* get_root()
{
	return root;
}

void write_block(int block_idx, uint8_t* data)
{
	int sector = (block_offset / 512) + ((block_idx * sizeof(Block) / 512));

	for(int i = 0; i < sizeof(Block) / 512; i++)
	{
		read_sector(sector + i, (uint8_t*)(data + (i * 512)));
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
	int used = 0;

	for(int i = 0; i < MAX_FILE_BLOCKS; i++)
	{
		if(file->block_pointers[i] == 0) break;

		used++;
	}

	return used;
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
	//FIXME might not work
	memcpy(entry->name, name, 14);

	Block* temp = load_block(block_idx);
	temp->used = 1;
	write_block(block_idx, (uint8_t*) temp);
	free(temp);

	//FIXME
	memcpy(inode->name, name, 14);
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

	//printf("Directory '%s' created at %d:%d\n", entry->name, directory_idx, inode_idx);

	return directory_idx;
}

void list_directory(Directory* directory)
{
	printf("Contents of ");
	printf(directory->self.name);
	printf(":\n");
	//DirEntry* entries = (DirEntry*) &directory->entries;
	DirEntry* entries = (DirEntry*) load_block(directory->self.block_pointers[0]);
	
	for(int i = 1; i < MAX_DIR_ENTRIES; i++)
	{
		DirEntry* entry = &entries[i];
		Inode* inode = get_inode(entry->inode_number);

		if(entry->inode_number == 0x00) break;

		printf("> ");
		printf(inode->name);
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
	
	//FIXME
	memcpy(entry->name, name, MAX_FILENAME_LEN);

	memcpy(inode->name, name, MAX_FILENAME_LEN);
	inode->access_time = 123;
	inode->creation_time = 123;
	inode->modification_time = 123;
	inode->permissions = 1;
	inode->size = sizeof(Block);
	inode->block_pointers[0] = root_block_idx;
	inode->type = type;
	inode->real_idx = inode_idx;

	root_block->used = 1;
	write_block(root_block_idx, (uint8_t*) root_block);

	//printf("%d;%d\n", cache->cached_directory->self.block_pointers[0], parent->self.block_pointers[0]);

	if(cache->cached_directory->self.block_pointers[0] == parent->self.block_pointers[0])
	{
		cache->inodes[cached_inodes_count].real_idx = inode_idx;
		cache->inodes[cached_inodes_count].data = inode;
		cached_inodes_count++;
	}

	entries[directory_entry_idx] = *entry;
	write_block(parent->self.block_pointers[0], (uint8_t*) entries);
	write_inode(inode);

	//printf("File '%s' created at %d:%d:%d\n", entry->name, directory_entry_idx, inode_idx, root_block_idx);	
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

Directory* get_dir_from_name(char* name, Directory* parent)
{

	Inode* dir = get_inode_name(parent, name);
	DirEntry* dir_entries = (DirEntry*) load_block(dir->block_pointers[0]);

	Directory* dir_d = malloc(sizeof(Directory));
	dir_d->self.block_pointers[0] = dir->block_pointers[0];
	memcpy(dir_d->self.name, dir->name, MAX_FILENAME_LEN);

	return dir_d;
}

void mercuryfs_init()
{
	block_offset = 512;
	inode_offset = 512 + (sizeof(Block) * MAX_BLOCKS);
	root_offset = 512 + ((sizeof(Block) * MAX_BLOCKS) + (sizeof(Inode) * MAX_INODES));
	cached_inodes_count = 1;


	header = malloc(512);
	read_sector(0, header);

	uint8_t* root_buffer = malloc(sizeof(Directory) + 512);
	for(int i = 0; i < (sizeof(Directory) + 512) / 512; i++)
	{
		read_sector((root_offset / 512) + i, root_buffer + i * 512);
	}

	root = malloc(sizeof(Directory));
	memcpy(root, root_buffer, sizeof(Directory));

	//FIXME not working (??)
	//write_header();
	//write_root();

	free(root_buffer);
}
