#include <syscalls.h>
#include <core/screen.h>
#include <core/types.h>
#include <exec/elf/elf_loader.h>
#include <exec/elf/symtable.h>
#include <memory/gdt.h>
#include <driver/driver.h>
#include <driver/ps2/ps2keyboard.h>
#include <driver/ata/ata.h>
#include <hardware/interrupts.h>
#include <hardware/pci.h>
#include <memory/mem_manager.h>
#include <memory/common.h>
#include <memory/paging.h>
//#include <fs/bootfs/bootfs.h>
//#include <fs/mercuryfs/mercuryfs.h>

#define ATA

extern uint8_t kernel_start;
extern uint8_t kernel_end;

uint32_t kernel_start_address;
uint32_t kernel_end_address;
uint32_t kernel_size;

uint32_t mem_manager_end;

void kernel_main()
{
	clear_screen();
	printf("<Quicksilver> Loading Quicksilver kernel... \n");

	segments_install_gdt();

	install_idt();

	printf("<Quicksilver> Registering syscalls\n");
	register_interrupt_handler(0x80, syscall);
	register_syscall_handler(0x4, (syscall_t) syscall_printf);

#ifdef ATA
	Disk ata0m = init_disk(0x1F0, true);
	if(identify_disk(&ata0m))
	{
		printf("<Quicksilver> Selected ata0m as default disk\n");
		set_default_disk(&ata0m);
	}

	Disk ata0s = init_disk(0x1F0, false);
	if(identify_disk(&ata0s))
	{
		printf("<Quicksilver> Selected ata0s as default disk\n");
		set_default_disk(&ata0s);
	}
#endif

	kernel_start_address = (uint32_t) &kernel_start;
	kernel_end_address = (uint32_t) &kernel_end;

	kernel_end_address = ((kernel_end_address / 4096) + 1) * 4096;

	kernel_size = kernel_end_address - kernel_start_address;
	print_memory_info();
	size_t mem_manager_size = 64 * 1024;
	init_memory_manager(kernel_start_address - mem_manager_size, mem_manager_size);
	init_memory_region(kernel_start_address - mem_manager_size, mem_manager_size);

	heap_init(0x200000, 0x200000);

	printf("<Quicksilver> Block manager size: 0x");
	print_hex((mem_manager_size >> 24) & 0xFF);
	print_hex((mem_manager_size >> 16) & 0xFF);
	print_hex((mem_manager_size >> 8) & 0xFF);
	print_hex(mem_manager_size & 0xFF);
	printf("\n");

	printf("<Quicksilver> Setting up paging\n");
	paging_enable();

	init_symtable();

	uint8_t keyboard_driver = create_driver(0x21, "PS2-Keyboard", 0, 
			ps2_kb_handle_interrupt, ps2_kb_enable, 
			ps2_kb_disable);

	enable_driver(keyboard_driver);

	printf("<Quicksilver> Searching PCI deivce drivers\n");
	pci_enumerate_devices(false);
	printf("<Quicksilver> PCI Initialization done\n");

	/*mercuryfs_init();
	Directory* sbin = get_dir_from_name("sbin", get_root());
	Inode* mercury = get_inode_name(sbin, "mercury");

	list_directory(sbin);

	uint8_t* mercury_buffer = malloc(40 * BLOCK_SIZE);
	memset(mercury_buffer, 0x0, sizeof(mercury_buffer));

	for(int i = 0; i < 40; i++)
	{
		Block* block = load_block(mercury->block_pointers[i]);
		memcpy((uint8_t*) mercury_buffer + ((BLOCK_SIZE - 1) * i), block->data, BLOCK_SIZE - 1);
		free(block);
	}

	void(*entry)();
	entry = image_load((char*) mercury_buffer, sizeof(mercury_buffer), true);
	free(mercury_buffer);
	entry();*/

	while(1);
}
