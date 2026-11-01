# Mercury

Mercury is a WIP x86-32 kernel.

## Building and booting
To build the kernel and create a bootable image: `make grub`
To run the kernel: `make qemu`

## TODO
- [ ] FAT32 R/W
- [ ] VFS
- [ ] initrd 
- [X] Ring 3 / Usermode
- [ ] Improved multitasking
- [ ] Supporting multiple cores
- [ ] Init process
- [ ] Syscalls
- [ ] Improve ELF32 features
- [ ] libc
