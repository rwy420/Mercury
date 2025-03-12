# Mercury

Mercury is a WIP x86-32 kernel. \
Report problems or give suggestions [here](https://github.com/rwy420/Mercury/issues) (Issue page)

**Features**
- FAT16 driver
- ELF32 App / SO loader
- Multitasking

**Work in progress**
- Linux like syscalls
- AHCI driver
- Networking (Drivers and lwip)
- Process isolation

 **Coming Features**
- Proper terminal
- Default system apps (ls, cat, echo...)
- Porting some apps made for Linux (Lua, Python...)

**Future ideas**
- Module system for drivers / services

**Screenshots** \
Multitasking and SO loading working: \
You can see two idle tasks counting up and a program using functions from LIBC.SO running
![Qemu Output](./docs/example-result01.png "Qemu Output")
![Code](./docs/example-code00.png "Code")
