# Mercury

Mercury is a WIP x86-32 kernel.

## Building and booting
To build the kernel and create a bootable image: `make grub`
To run the kernel: `make qemu`

## Features
- ELF32 Executable / DL loading
- Multitasking

## TODO
- [ ] FAT32 R/W
- [ ] VFS 
- [X] Ring 3 / Usermode
- [ ] Improved multitasking
- [ ] Supporting multiple cores
- [ ] Init process
- [ ] Syscalls
- [ ] libc
