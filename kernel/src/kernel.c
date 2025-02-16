#include <syscalls.h>
#include <common/screen.h>
#include <common/types.h>
#include <exec/elf/elf_loader.h>
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
	register_interrupt_handler(0x80, syscall);
	register_syscall_handler(1, (syscall_t) syscall_printf);
	register_syscall_handler(80, (syscall_t) kernel_switch_back);

#ifdef ATA
	Disk ata0m = init_disk(0x1F0, true);
	if(identify_disk(&ata0m))
	{
		printf("<Mercury> Selected ata0m as default disk\n");
		set_default_disk(&ata0m);
	}

	Disk ata0s = init_disk(0x1F0, false);
	if(identify_disk(&ata0s))
	{
		printf("<Mercury> Selected ata0s as default disk\n");
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

	printf("<Mercury> Block manager size: 0x");
	print_hex((mem_manager_size >> 24) & 0xFF);
	print_hex((mem_manager_size >> 16) & 0xFF);
	print_hex((mem_manager_size >> 8) & 0xFF);
	print_hex(mem_manager_size & 0xFF);
	printf("\n");

	printf("<Mercury> Setting up paging\n");
	//paging_enable();#include "syscall.h"

void syscall(int syscall_number, void* arg) {
    asm volatile (
        "int $0x80"             // Trigger syscall interrupt
        :                       // No outputs
        : "a"(syscall_number),  // Syscall number in EAX
          "b"(arg)              // Argument in EBX
        : "memory"
    );
}


	init_symtable();

	uint8_t keyboard_driver = create_driver(0x21, "PS2-Keyboard", 0, 
			ps2_kb_handle_interrupt, ps2_kb_enable, 
			ps2_kb_disable);

	enable_driver(keyboard_driver);

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
	uint8_t* buffer = malloc(13512);
	fat16_read(fd, buffer, 13512);

	void(*entry)();
	entry = image_load(buffer, sizeof(buffer), true);
	free(buffer);

	execute_user_mode(entry);

	while(1);
}
