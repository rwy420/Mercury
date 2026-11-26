#include <vm.h>
#include <exec/elf32/elf32hash.h>
#include <exec/elf32/elf32dynlib.h>
#include <printf.h>
#include <memory/common.h>
#include <fs/vfs/vfs.h>
#include <memory/heap.h>

Elf32_Load_Info* elf32_handle_dtneeded(const char* nameoff, Elf32_Load_Info* load_info)
{
	const char* name = (char*) (load_info->offset + load_info->dynstr_off + (uint32_t) nameoff);
	size_t name_len = strlen(name);
	char path[5 + name_len]; // /lib/ + strlen(name);
	memcpy(path, "/lib/", 5);
	memcpy(&path[5], name, name_len + 1/* Should include '\0'*/); 

	uint32_t lib_file = vfs_open(path);

	if(!lib_file) return 0;

	size_t lib_size = vfs_get_size(lib_file);
	uint8_t* lib_buffer = kmalloc(lib_size);
	vfs_read(lib_file, lib_buffer, lib_size);

	Elf32_Load_Info* lib = elf32_load(lib_buffer, lib_size, 0 /* Will use parent's pd if null*/, load_info);
	if(!lib) return 0;

	dbgprintf("Loaded %s at 0x%x\n", path, lib->offset);

	kfree(lib_buffer);

	return lib;
}

void* elf32_total_lookup(const char* name, Elf32_Load_Info* load_info)
{
	if(load_info->parent) load_info = load_info->parent; // Always use the parent executable because it contains the array off all the libs
	
	for(int i = 0; i < load_info->needed_count; i++)
	{
		Elf32_Load_Info* lib = load_info->needed[i];
		return elf32_lib_lookup(name, lib);
	}
	
	return 0;
}

void* elf32_lib_lookup(const char* name, Elf32_Load_Info* load_info)
{
	Elf32_Sym* sym = elf32_hash_lookup(name, load_info);
	if(!sym) return 0;

	return (void*) (load_info->offset + sym->st_value);
}
