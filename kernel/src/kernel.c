#include <printf.h>
#include <thread.h>
#include <multiboot.h>
#include <syscalls.h>
#include <fd.h>
#include <vesa.h>
#include <process.h>
#include <multitasking.h>
#include <common/types.h>
#include <exec/elf32/elf32loader.h>
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
#include <fs/disk.h>
#include <faults.h>
#include <fs/vfs/vfs.h>
#include <fs/initrd.h>
#include <printf.h>
#define ATA
//#define STARTTEXT

// Variables from the linker script
extern uint8_t ld_kernel_start;
extern uint8_t ld_kernel_end;

extern Disk* g_default_disk;
extern uint32_t g_ms_since_init;

uint32_t kernel_start_address;
uint32_t kernel_end_address;
uint32_t mem_manager_end;

VesaInfo g_vesa_info;
multiboot_module_t* multiboot_modules;

extern PageDirectory* g_kernel_pd;

void v_kernel_start();
void kthread_init();

char* a = "Testing! This won't overwrite anything now!!\n";
char* b = "ARGV";
char* c = "ABCD1234";

void kernel_init(uint32_t multiboot, uint32_t magic)
{
	if(!mb_confirm(multiboot, magic));

	mb_get_vesa(&g_vesa_info);
   
	if(!paging_init()); //TODO error handling

	// Calculate the address of v_kernel_start in the higher half once mapped
	void(*v_kernel)(void) = (void*) 0xC0000000 + (uint32_t) &v_kernel_start;

	size_t heap_size = 0x1000000;
	uint32_t heap_start = 0xC0200000;

	uint32_t heap_max = heap_init(heap_start, heap_size);

	vesa_init();
	// Identity map the VESA framebuffer
	vesa_map(g_kernel_pd);
	vesa_clear();

	kprintf("<GRUB> VESA resolution set to %ux%u\n", g_vesa_info.fb_width, g_vesa_info.fb_height);

	multiboot_modules = mb_init_modules();

	kprintf("<Mercury> Loading Mercury kernel... \n");
	kprintf("<Mercury> kernel_start is mapped to 0x%x\n", (uint32_t) v_kernel);
	kprintf("<Mercury> Kernel heap of %u MB initialized\n", (uint32_t) heap_size / 0x400 / 0x400);
	kprintf("<Mercury> Kernel heap mapped up to 0x%x\n", (uint32_t) heap_max);
	kprintf("<Mercury> Switching to higher half\n");

	// Switching to the kernel function living in the higher half
	v_kernel();
}

void v_kernel_start()
{
	// Stack needs to be in higher half too
	asm volatile("add $0xC0000000, %ESP");
	segments_install_gdt();
	install_idt();
	pit_init(250);
	
	fd_init();

	register_syscall_handler(0x01, (syscall_t) syscall_exit);
	register_syscall_handler(0x03, (syscall_t) syscall_read);
	register_syscall_handler(0x04, (syscall_t) syscall_write);
	register_syscall_handler(0x05, (syscall_t) syscall_open);
	register_syscall_handler(0x06, (syscall_t) syscall_close);
	register_syscall_handler(0x13, (syscall_t) syscall_lseek);
	kprintf("<Mercury> Syscalls registered\n");

	register_interrupt_handler(0x06, (isr_t) handle_invalid_opcode);
	register_interrupt_handler(0x0D, (isr_t) handle_general_protection_fault);
	register_interrupt_handler(0x0E, (isr_t) handle_page_fault);
	register_interrupt_handler(0x80, (isr_t) syscall);

#ifdef ATA
	Disk ata0m = init_disk(0x1F0, true);
	if(identify_disk(&ata0m))
	{
		kprintf("<Mercury> Selected ata0m as default disk\n");
		g_default_disk = &ata0m;
	}

	Disk ata0s = init_disk(0x1F0, false);
	if(identify_disk(&ata0s))
	{
		kprintf("<Mercury> Selected ata0s as default disk\n");
		g_default_disk = &ata0s;
	}
#endif

	kernel_start_address = (uint32_t) &ld_kernel_start;
	kernel_end_address = (uint32_t) &ld_kernel_end;

	kernel_end_address = ((kernel_end_address / 4096) + 1) * 4096;

	size_t frame_allocator_size = 0x40000000;
	uint32_t frame_allocator_start = 0x40000000;

	frame_allocator_init(frame_allocator_start, frame_allocator_size);
	kprintf("<Mercury> Frame allocator of %u MB initialized\n", (uint32_t) frame_allocator_size / 0x400 / 0x400);

	tasks_init();
	// Continuing kernel initialization in a kernel task
	kthread_create("KERNEL", kthread_init);
	pit_set_schedule(true);

	while(1);
}

void kthread_init()
{
	init_drivers();

	kprintf("<PCI> Enumerating PCI devices\n");
	pci_enumerate_devices();

	kprintf("<PCI> Initializing PCI devices\n");
	pci_init_devices();

	vfs_init();
	//load_initrd();
	if(!read_mbr()) kprintf("<Mercury> Could not read the MBR\n");
	
	uint8_t ps2_keyboard = create_driver("PS2-KB", KEYBOARD, NULL_PTR, ps2_kb_enable, ps2_kb_disable, NULL_PTR);
	enable_all_drivers();

	kprintf("<Mercury> Startup done\n");
#ifdef STARTTEXT
	vesa_clear();
	kprintf("  __  __                                \n |  \\/  |                               \n | \\  / | ___ _ __ ___ _   _ _ __ _   _ \n | |\\/| |/ _ \\ '__/ __| | | | '__| | | |\n | |  | |  __/ | | (__| |_| | |  | |_| |\n |_|  |_|\\___|_|  \\___|\\__,_|_|   \\__, |\n                                   __/ |\nBy Arjan (rwy)                    |___/ \n");
#endif
	
	char** argv = kmalloc(sizeof(char*) * 3); 
	argv[0] = a;
	argv[1] = b;
	argv[2] = c;

	//TODO: This should become execve to replace this task by INIT.ELF
	process_create("/SBIN/INIT.ELF", 3, argv);

	while(1);
}
