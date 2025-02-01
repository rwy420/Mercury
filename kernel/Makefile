GPPPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions \
			-fno-leading-underscore -fno-stack-protector -Wwrite-strings
ASPARAMS = --32
LDPARAMS = -melf_i386

OBJECTS = build/entry.o build/kernel.o build/screen.o build/gdt.o build/port.o build/interrupts.o \
		  build/interrupt_handler.o build/gdt_asm.o build/pci.o build/ps2keyboard.o build/driver.o build/pic.o \
		  build/common.o build/mem_manager.o build/paging.o build/paging_asm.o build/syscalls.o build/ata.o \
		  build/bootfs.o build/elf_loader.o build/elf.o build/symtable.o build/vga.o build/sata.o build/ahci.o \
		  build/mercuryfs.o

GRUB_OBJECTS = build/grub_entry.o build/kernel.o build/screen.o build/gdt.o build/port.o build/interrupts.o \
		  build/interrupt_handler.o build/gdt_asm.o build/pci.o build/ps2keyboard.o build/driver.o build/pic.o \
		  build/common.o build/mem_manager.o build/paging.o build/paging_asm.o build/syscalls.o build/ata.o \
		  build/bootfs.o build/elf_loader.o build/elf.o build/symtable.o build/vga.o build/sata.o build/ahci.o \
		  build/mercuryfs.o


build/%.o: src/%.c
	gcc $(GPPPARAMS) -o $@ -c $<

build/%.o: src/core/%.c
	gcc $(GPPPARAMS) -o $@ -c $<

build/%.o: src/hardware/%.c
	gcc $(GPPPARAMS) -o $@ -c $<

build/%.o: src/driver/%.c
	gcc $(GPPPARAMS) -o $@ -c $<

build/%.o: src/driver/ps2/%.c
	gcc $(GPPPARAMS) -o $@ -c $<

build/%.o: src/driver/vga/%.c
	gcc $(GPPPARAMS) -o $@ -c $<

build/%.o: src/driver/sata/%.c
	gcc $(GPPPARAMS) -o $@ -c $<

build/%.o: src/driver/ata/%.c
	gcc $(GPPPARAMS) -o $@ -c $<

build/%.o: src/fs/bootfs/%.c
	gcc $(GPPPARAMS) -o $@ -c $<

build/%.o: src/fs/mercuryfs/%.c
	gcc $(GPPPARAMS) -o $@ -c $<

build/%.o: src/memory/%.c
	gcc $(GPPPARAMS) -o $@ -c $<

build/%.o: src/exec/elf/%.c
	gcc $(GPPPARAMS) -o $@ -c $<

build/%.o: arch/x86/%.s
	as $(ASPARAMS) -o $@ $<

build/%.o: arch/grub_x86/%.s
	as $(ASPARAMS) -o $@ $<

build/%.o: arch/x86/%.asm
	nasm -felf32 -o $@ $<

kernel.bin: linker.ld $(OBJECTS)
	ld $(LDPARAMS) -T $< -o $@ $(OBJECTS)

grub: grub_linker.ld $(GRUB_OBJECTS)
	ld $(LDPARAMS) -T $< -o grub-kernel.bin $(GRUB_OBJECTS)

clean:
	rm -rf ./build/*
	rm kernel.bin
