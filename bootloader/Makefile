KFILE = ../kernel/kernel.bin
KSIZE := $(shell stat --format=%s $(KFILE))

.PHONY: all image stage1 stage2 clean

image: build/image.img

build/image.img: stage1 stage2
	dd if=/dev/zero of=build/image.img  bs=512 count=2880
	dd if=build/stage1.bin of=build/image.img conv=notrunc
	dd if=build/stage2.bin of=build/image.img conv=notrunc oflag=seek_bytes seek=512

stage1: build/stage1.bin

build/stage1.bin:
	nasm src/stage1/boot.asm -f bin -o build/stage1.bin

stage2: build/stage2.bin

build/stage2.bin:
	#nasm src/stage2/main.asm -f bin -o build/stage2.bin
	nasm src/stage2/main.asm -f elf -o build/stage2main_asm.o
	gcc -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-pic -fno-pie -fno-stack-protector \
		-c src/stage2/main.c -o build/stage2main_c.o
	gcc -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-pic -fno-pie -fno-stack-protector \
		-c src/stage2/screen.c -o build/stage2screen_c.o
	gcc -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-pic -fno-pie -fno-stack-protector \
		-c src/stage2/ata.c -o build/stage2ata_c.o
	gcc -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-pic -fno-pie -fno-stack-protector \
		-c src/stage2/common.c -o build/stage2common_c.o
	gcc -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-pic -fno-pie -fno-stack-protector \
		-c src/stage2/fat16.c -o build/stage2fat16_c.o
	gcc -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-pic -fno-pie -fno-stack-protector \
		-c src/stage2/vesa.c -o build/vesa_c.o
	@echo "Dynamically adjusted kernel size to $(KSIZE)"
	ld -m elf_i386 -T linker.ld --defsym ksize=$(KSIZE) -o build/stage2.bin build/stage2main_asm.o build/stage2main_c.o \
		build/stage2screen_c.o build/stage2ata_c.o build/stage2common_c.o build/stage2fat16_c.o build/vesa_c.o

clean:
	rm -rf build/*
