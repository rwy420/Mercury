#include "./screen.h"
#include "./ata.h"
#include "./fat16.h"

Disk* default_disk;
extern char ksize;

void stage2_main()
{
	clear_screen();
	printf("BOOT DISK INIT...\n");

	Disk ata0m = init_disk(0x1F0, true);
	if(identify_disk(&ata0m))
	{
		printf("USING ATA0M AS BOOT DISK\n");
		default_disk = &ata0m;
	}

	Disk ata0s = init_disk(0x1F0, false);
	if(identify_disk(&ata0s))
	{
		printf("USING ATA0S AS BOOT DISK\n");
		default_disk = &ata0s;
	}

	void* kernel_buffer = (void*) 0x100000 - 0x1000;

	fat16_init(0);
	fat16_read("/BOOT/KERNEL0.ELF", 'r', kernel_buffer, (unsigned long) &ksize);

	void(*entry)() = (void*) 0x100000;
	entry();
	
	
	printf("SOMETHING BAD HAS HAPPENED AND YOU HAVE REACHED THE BOOTLOADER FALLBACK LOOP\n");
	while(1);
}
