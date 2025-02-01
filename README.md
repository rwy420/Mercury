# MercuryOS

MercuryOS is an operating system relying on my kernel `Quicksilver`. 
Check kernel/TODO.txt for future features.
Report problems or give suggestions [Here](https://github.com/rwy420/MercuryOS/issues) (Issue page)

To build and run the kernel (VirtualBox):

	make setup
	make vdi
Then just simply add `build/disk.vdi` as a hard drive to an empty virtual machine in VirtualBox.

**Boot process**
The first stage of the bootloader is located in the MBR's boot section and therefore gets automatically loaded and executed. Said stage will then load the next 32 sectors containing the second stage and the kernel after the BootFS header. It will then enter the second stage which will enter 32 bit mode and relocate the kernel to a more suitable location in memory (100000h). The first stage couldn't have done that because it's in 16 bit mode.
The second stage will simply enter the kernel's `entry` which sets up the stack and jumps into the kernel's `kernel_main` from where it will get the hardware ready to then finally load and execute the actual OS's main function.

*Last update: February 1st 2025 - Mercury 1.0
