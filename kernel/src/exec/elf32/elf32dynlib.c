#include <exec/elf32/elf32dynlib.h>
#include <printf.h>
#include <memory/common.h>
#include <fs/vfs/vfs.h>
#include <memory/heap.h>

int elf32_handle_dtneeded(char* nameoff, Elf32_Load_Info* load_info)
{
	char* name = (char*) (load_info->offset + load_info->dynstr_off + (uint32_t) nameoff);
	size_t name_len = strlen(name);
	char path[5 + name_len]; // /lib/ + strlen(name);
	memcpy(path, "/lib/", 5);
	memcpy(&path[5], name, name_len + 1/* Should include \0*/); 

	uint32_t lib_file = vfs_open(path);

	if(!lib_file) return 0;

	size_t lib_size = vfs_get_size(lib_file);
	uint8_t* lib_buffer = kmalloc(lib_size);
	vfs_read(lib_file, lib_buffer, lib_size);

	Elf32_Load_Info* lib = elf32_load(lib_buffer, lib_size, 0 /* Will use parent's pd if null*/, load_info);

	kprintf("Loaded %s at 0x%x", path, lib->offset);

	kfree(lib_buffer);

	return 0;
}
