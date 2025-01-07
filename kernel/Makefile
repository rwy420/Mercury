GPPPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions \
			-fno-leading-underscore -fno-stack-protector -Wwrite-strings
ASPARAMS = --32
LDPARAMS = -melf_i386

OBJECTS = build/entry.o build/kernel.o build/screen.o build/gdt.o build/port.o build/interrupts.o \
		  build/interrupt_handler.o build/gdt_asm.o build/pci.o build/ps2keyboard.o build/driver.o build/pic.o \
		  build/common.o build/mem_manager.o build/paging.o build/paging_asm.o build/syscalls.o build/ata.o \
		  build/bootfs.o build/elf_loader.o build/elf.o

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

build/%.o: src/driver/disk/%.c
	gcc $(GPPPARAMS) -o $@ -c $<

build/%.o: src/driver/ata/%.c
	gcc $(GPPPARAMS) -o $@ -c $<

build/%.o: src/fs/bootfs/%.c
	gcc $(GPPPARAMS) -o $@ -c $<

build/%.o: src/memory/%.c
	gcc $(GPPPARAMS) -o $@ -c $<

build/%.o: src/core/elf/%.c
	gcc $(GPPPARAMS) -o $@ -c $<

build/%.o: asm/%.s
	as $(ASPARAMS) -o $@ $<

build/%.o: asm/%.asm
	nasm -felf32 -o $@ $<

kernel.bin: linker.ld $(OBJECTS)
	ld $(LDPARAMS) -T $< -o $@ $(OBJECTS)

clean:
	rm -rf ./build/*
	rm kernel.bin
