#include <syscalls.h>
#include <fd.h>
#include <vesa.h>
#include <shell.h>
#include <process.h>
#include <multitasking.h>
#include <common/screen.h>
#include <common/types.h>
#include <exec/elf/elf_loader.h>
#include <exec/elf/symtable.h>
#include <memory/gdt.h>
#include <driver/driver.h>
#include <driver/vga/vga.h>
#include <driver/ps2/ps2keyboard.h>
#include <driver/ata/ata.h>
#include <hardware/interrupts.h>
#include <hardware/pci.h>
#include <hardware/pit.h>
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

VesaInfoBlock g_vesa_info_block;

void kernel_main(VesaInfoBlock vesa_info_block)
{
	g_vesa_info_block.fb = vesa_info_block.fb;
	g_vesa_info_block.fb_width = vesa_info_block.fb_width;
	g_vesa_info_block.fb_height = vesa_info_block.fb_height;
	vesa_init();

	clear_screen();
	printf("<Mercury> Loading Mercury kernel... \n");

	segments_install_gdt();
	install_idt();
	pit_init(10);

	printf("<Mercury> Registering syscalls\n");
	fd_init();

	register_syscall_handler(0x01, (syscall_t) syscall_exit);
	register_syscall_handler(0x03, (syscall_t) syscall_read);
	register_syscall_handler(0x04, (syscall_t) syscall_write);
	register_syscall_handler(0x05, (syscall_t) syscall_open);
	register_syscall_handler(0x06, (syscall_t) syscall_close);
	register_syscall_handler(0x13, (syscall_t) syscall_lseek);

	register_interrupt_handler(14, (isr_t) handle_page_fault);

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

	print_hex32(vesa_info_block.fb);

	printf("<Mercury> Setting up paging\n");
	paging_init();
	vesa_map();

	symtable_init();

	init_drivers();
	uint8_t ps2_keyboard = create_driver("PS2-KB", KEYBOARD, NULL_PTR, ps2_kb_enable, ps2_kb_disable, NULL_PTR);

	printf("<Mercury> Searching PCI deivce drivers\n");
	pci_enumerate_devices(false);
	printf("<Mercury> PCI Initialization done\n");

	enable_all_drivers();

	storage_dev_t* fat_dev = kmalloc(sizeof(storage_dev_t));
	fat_dev->read = _read;
	fat_dev->read_byte = _read_byte;
	fat_dev->seek = _seek;
	fat_dev->write = _write;
	fat16_init(fat_dev , 0);

	//tasks_init();
	register_interrupt_handler(0x20, schedule);

	while(1);
}
