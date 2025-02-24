GCCPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions \
			-fno-leading-underscore -fno-stack-protector -Wwrite-strings \
			-Os -ffunction-sections -fdata-sections -mno-sse -msoft-float -finline-small-functions

ASPARAMS = --32
LDPARAMS = -melf_i386

OBJECTS = build/entry.o build/kernel.o build/screen.o build/gdt.o build/port.o build/interrupts.o \
		  build/interrupt_handler.o build/gdt_asm.o build/pci.o build/ps2keyboard.o build/driver.o build/pic.o \
		  build/common.o build/mem_manager.o build/paging.o build/syscalls.o build/ata.o \
		  build/elf_loader.o build/elf.o build/symtable.o build/sata.o build/ahci.o \
		  build/fat16.o build/fat16_path.o build/fat16_priv.o build/fat16_rootdir.o build/fat16_subdir.o \
		  build/usermode.o build/vga.o build/fd.o build/elf_dl.o build/shell.o build/pit.o build/multitasking.o

GRUB_OBJECTS = build/grub_entry.o build/kernel.o build/screen.o build/gdt.o build/port.o build/interrupts.o \
		  build/interrupt_handler.o build/gdt_asm.o build/pci.o build/ps2keyboard.o build/driver.o build/pic.o \
		  build/common.o build/mem_manager.o build/paging.o build/paging_asm.o build/syscalls.o build/ata.o \
		  build/bootfs.o build/elf_loader.o build/elf.o build/symtable.o build/sata.o build/ahci.o \
		  build/mercuryfs.o


build/%.o: src/%.c
	gcc $(GCCPARAMS) -o $@ -c $<

build/%.o: src/common/%.c
	gcc $(GCCPARAMS) -o $@ -c $<

build/%.o: src/hardware/%.c
	gcc $(GCCPARAMS) -o $@ -c $<

build/%.o: src/driver/%.c
	gcc $(GCCPARAMS) -o $@ -c $<

build/%.o: src/driver/ps2/%.c
	gcc $(GCCPARAMS) -o $@ -c $<

build/%.o: src/driver/vga/%.c
	gcc $(GCCPARAMS) -o $@ -c $<

build/%.o: src/driver/sata/%.c
	gcc $(GCCPARAMS) -o $@ -c $<

build/%.o: src/driver/ata/%.c
	gcc $(GCCPARAMS) -o $@ -c $<

build/%.o: src/fs/fat16/%.c
	gcc $(GCCPARAMS) -o $@ -c $<

build/%.o: src/memory/%.c
	gcc $(GCCPARAMS) -o $@ -c $<

build/%.o: src/exec/elf/%.c
	gcc $(GCCPARAMS) -o $@ -c $<

build/%.o: src/exec/%.c
	gcc $(GCCPARAMS) -o $@ -c $<

build/%.o: asm/%.s
	as $(ASPARAMS) -o $@ $<

build/%.o: asm/%.asm
	nasm -felf32 -o $@ $<

kernel.bin: linker.ld $(OBJECTS)
	@echo "Mercury Size: $(MSIZE)"
	ld $(LDPARAMS) -T $< -o $@ $(OBJECTS)
	strip --strip-all kernel.bin

grub: grub_linker.ld $(GRUB_OBJECTS)
	ld $(LDPARAMS) -T $< -o grub-kernel.bin $(GRUB_OBJECTS)

clean:
	rm -rf ./build/*
	rm kernel.bin
