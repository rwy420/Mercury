# Mercury

Mercury is a WIP x86-32 kernel.

## Building and booting
To build the kernel and create a bootable image: `make grub`
To run the kernel: `make qemu`

## TODO
- [X] Ring 3 / Usermode (May 15th 2026)
- [ ] Improve ELF32 features [WIP since Nov 12th 2026]
- [ ] Syscalls
- [ ] Init process [WIP since Nov 7th 2026]
- [ ] libc
- [ ] FAT32 R/W
- [ ] VFS [WIP since May 16th 2026] 
- [ ] Improved multitasking
- [ ] Supporting multiple cores
