#include <syscalls.h>
#include <fd.h>
#include <common/screen.h>
#include <common/types.h>
#include <exec/elf/elf_loader.h>
#include <exec/elf/elf_so.h>
#include <exec/elf/symtable.h>
#include <exec/usermode.h>
#include <memory/gdt.h>
#include <driver/driver.h>
#include <driver/vga/vga.h>
#include <driver/ps2/ps2keyboard.h>
#include <driver/ata/ata.h>
#include <hardware/interrupts.h>
#include <hardware/pci.h>
#include <memory/mem_manager.h>
#include <memory/common.h>
#include <memory/paging.h>
#include <fs/fat16/fat16.h>

#define ATA

extern uint8_t kernel_start;
extern uint8_t kernel_end;
extern Disk* g_default_disk;

uint32_t kernel_start_address;
uint32_t kernel_end_address;
uint32_t kernel_size;


uint32_t mem_manager_end;

void kernel_main()
{
	clear_screen();
	printf("<Mercury> Loading Mercury kernel... \n");

	segments_install_gdt();
	install_idt();

	printf("<Mercury> Registering syscalls\n");
	fd_init();
	register_interrupt_handler(0x80, syscall);
	register_syscall_handler(4, (syscall_t) syscall_write);
	register_syscall_handler(80, (syscall_t) kernel_switch_back);

#ifdef ATA
	Disk ata0m = init_disk(0x1F0, true);
	if(identify_disk(&ata0m))
	{
		printf("<Mercury> Selected ata0m as default disk\n");
		g_default_disk = &ata0m;
	}

	Disk ata0s = init_disk(0x1F0, false);
	if(identify_disk(&ata0s))
	{
		printf("<Mercury> Selected ata0s as default disk\n");
		g_default_disk = &ata0s;
	}
#endif

	kernel_start_address = (uint32_t) &kernel_start;
	kernel_end_address = (uint32_t) &kernel_end;

	kernel_end_address = ((kernel_end_address / 4096) + 1) * 4096;

	kernel_size = kernel_end_address - kernel_start_address;
	print_memory_info();
	size_t mem_manager_size = 64 * 1024;
	size_t heap_size = 0x200000;

	heap_init(0x200000, heap_size);

	printf("<Mercury> Block manager size: 0x");
	print_hex((mem_manager_size >> 24) & 0xFF);
	print_hex((mem_manager_size >> 16) & 0xFF);
	print_hex((mem_manager_size >> 8) & 0xFF);
	print_hex(mem_manager_size & 0xFF);
	printf("\n");



	printf("<Mercury> Setting up paging\n");
	paging_enable();

	init_symtable();

	init_drivers();
	uint8_t ps2_keyboard = create_driver("PS2-KB", KEYBOARD, NULL_PTR, ps2_kb_enable, ps2_kb_disable);
	enable_driver(ps2_keyboard);

	printf("<Mercury> Searching PCI deivce drivers\n");
	pci_enumerate_devices(false);
	printf("<Mercury> PCI Initialization done\n");

	storage_dev_t* fat_dev = malloc(sizeof(storage_dev_t));
	fat_dev->read = _read;
	fat_dev->read_byte = _read_byte;
	fat_dev->seek = _seek;
	fat_dev->write = _write;
	fat16_init(fat_dev , 0);


	char fname[11];
	uint32_t i = 0;
	while(fat16_ls(&i, fname, "/") == 1)
	{
		printf(fname);
		printf("\n");
	}

	int fd = fat16_open("/BIN/TEST.ELF", 'r');
	int size = fat16_size("/BIN/TEST.ELF"); 
	uint8_t* buffer = malloc(size);
	fat16_read(fd, buffer, size);

	void(*entry)();
	entry = image_load(buffer, sizeof(buffer), true);
	free(buffer);
	
	execute_user_mode(entry);

	/*int dl_handle = dlopen("/LIB/LIBC.SO");
	void* _printf = dlsym(dl_handle, "printf");

	print_hex32((uint32_t) _printf);

	void (*func)(char* str) = _printf;
	func("Hello, World");
	dlclose(dl_handle);*/

	while(1);
}
