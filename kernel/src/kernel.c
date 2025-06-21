#include "hardware/usb/usb.h"
#include "multiboot.h"
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
#include <memory/heap.h>
#include <memory/common.h>
#include <memory/paging.h>
#include <memory/frames.h>
#include <fs/fat16/fat16.h>

#define ATA

extern uint8_t ld_kernel_start;
extern uint8_t ld_kernel_end;
extern Disk* g_default_disk;
extern uint32_t g_ms_since_init;

uint32_t kernel_start_address;
uint32_t kernel_end_address;
uint32_t mem_manager_end;

VesaInfoBlock g_vesa_info_block;

extern PageDirectory* g_kernel_pd;

void v_kernel_start();

void kernel_init(uint32_t multiboot, uint32_t magic)
{
	multiboot_info_t* mb = (multiboot_info_t*) multiboot;

	g_vesa_info_block.fb = mb->framebuffer_addr;
	g_vesa_info_block.fb_height = mb->framebuffer_height;
	g_vesa_info_block.fb_width = mb->framebuffer_width;
   
	if(!paging_init()); //TODO error handling
										
	void(*v_kernel)(void) = (void*) 0xC0000000 + (uint32_t) &v_kernel_start;

	size_t heap_size = 0x1000000;
	uint32_t heap_start = 0xC0200000;

	uint32_t heap_max = heap_init(heap_start, heap_size);

	vesa_init();
	vesa_map(g_kernel_pd);
	clear_screen();

	printf("<Mercury> Loading Mercury kernel... \n");
	
	printf("<Mercury> kernel_start is mapped to 0x");
	print_hex32((uint32_t) v_kernel);
	printf("\n<Mercury> Kernel heap of ");
	print_uint32_t(heap_size / 0x400 / 0x400);
	printf(" MB initialized\n<Mercury> Kernel heap mapped up to 0x");
	print_hex32(heap_max);
	printf("\n<Mercury> Switching to higher half\n");

	v_kernel();
}

void v_kernel_start()
{
	segments_install_gdt();
	install_idt();
	pit_init(10);

	fd_init();

	register_syscall_handler(0x01, (syscall_t) syscall_exit);
	register_syscall_handler(0x03, (syscall_t) syscall_read);
	register_syscall_handler(0x04, (syscall_t) syscall_write);
	register_syscall_handler(0x05, (syscall_t) syscall_open);
	register_syscall_handler(0x06, (syscall_t) syscall_close);
	register_syscall_handler(0x13, (syscall_t) syscall_lseek);
	printf("<Mercury> Syscalls registered\n");

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

	kernel_start_address = (uint32_t) &ld_kernel_start;
	kernel_end_address = (uint32_t) &ld_kernel_end;

	kernel_end_address = ((kernel_end_address / 4096) + 1) * 4096;

	size_t frame_allocator_size = 0x40000000;
	uint32_t frame_allocator_start = 0x40000000;

	frame_allocator_init(frame_allocator_start, frame_allocator_size);
	printf("<Mercury> Frame allocator of ");
	print_uint32_t(frame_allocator_size / 0x400 / 0x400);
	printf(" MB initialized\n");

	tasks_init();
	symtable_init();
	init_drivers();

	printf("<PCI> Enumerating PCI devices\n");
	pci_enumerate_devices();
	
	printf("<USB> Initializing USB\n");
	usb_init();

	printf("<PCI> Initializing PCI devices\n");
	pci_init_devices();

	/*storage_dev_t* fat_dev = kmalloc(sizeof(storage_dev_t));
	fat_dev->read = _read;
	fat_dev->read_byte = _read_byte;
	fat_dev->seek = _seek;
	fat_dev->write = _write;
	fat16_init(fat_dev , 0);*/

	uint8_t ps2_keyboard = create_driver("PS2-KB", KEYBOARD, NULL_PTR, ps2_kb_enable, ps2_kb_disable, NULL_PTR);
	enable_all_drivers();

	pit_set_schedule(true);
		
	printf_color("<Mercury> Startup done\n", COLOR_GREEN, COLOR_BLACK);

	while(1);
}
