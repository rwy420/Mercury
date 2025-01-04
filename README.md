# MercuryOS

MercuryOS is a basic kernel written in C. Fully functional with VirtualBox!
Check kernel/TODO.txt for future features.
Report problems or give suggestions [Here](https://github.com/rwy420/MercuryOS/issues) (Issue page)

To run the kernel (VirtualBox):

    make image
    VBoxManage convertdd ./mercury_image.img mercury.vdi --format VDI
Then just simply add the VDI as a hard drive to an empty virtual machine in VirtualBox.

**Boot process**
The first stage of the bootloader is located in the MBR's boot section and therefore gets automatically loaded and executed. Said stage will then load the next 32 sectors containing the second stake and the kernel after the BootFS header. It will then enter the second stage which will enter 32 bit mode and relocate the kernel to a more suitable location in memory (100000h). The first stage couldn't have done that because it's in 16 bit mode.
The second stage will simply enter the kernel's `entry` which simply sets up the stack and jumps into the kernel's `kernel_main` from where it will install the GDT, identify any (ATA) disks and so on..

*Last update: January 4th 2025 - Kernel 1.0*
