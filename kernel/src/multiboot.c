#include <multiboot.h>
#include <common/screen.h>

multiboot_info_t* mb;

int mb_confirm(uint32_t multiboot, uint32_t magic)
{
	mb = (multiboot_info_t*) multiboot;

	return 0;
}

void mb_get_vesa(VesaInfo* vesa_info)
{
	vesa_info->fb = mb->framebuffer_addr;
	vesa_info->fb_height = mb->framebuffer_height;
	vesa_info->fb_width = mb->framebuffer_width;
}

void mb_init_modules()
{
	printf("<GRUB> Loaded ");
	print_uint32_t(mb->mods_count);
	printf(" module(s)\n");

	multiboot_module_t* mod;
	uint32_t mod_idx = 0;
	for(mod_idx = 0, mod = (multiboot_module_t*) mb->mods_addr; mod_idx < mb->mods_count; mod_idx++, mod++)
	{
		print_hex32(mod->mod_start);
		printf(" ");
		print_hex32(mod->mod_end);
		printf("\n");
	}
}
