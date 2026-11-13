# Mercury

Mercury is a WIP x86-32 kernel.

## Building and booting
To build the kernel and create a bootable image: `make grub`
To run the kernel: `make qemu`

## TODO
- [X] Ring 3 / Usermode
- [ ] Improve ELF32 features [WIP]
- [ ] Syscalls [WIP]
- [ ] Init process [WIP]
- [ ] libc
- [ ] FAT32 R/W
- [ ] VFS [WIP] 
- [ ] Improved multitasking
- [ ] Supporting multiple cores
