ASM = nasm

.PHONY: all image stage1 stage2 clean

image: build/image.img

build/image.img: stage1 stage2
	dd if=/dev/zero of=build/image.img  bs=512 count=2880
	dd if=build/stage1.bin of=build/image.img conv=notrunc
	dd if=build/stage2.bin of=build/image.img conv=notrunc oflag=seek_bytes seek=4608
	dd if=kernel.bin of=build/image.img conv=notrunc oflag=seek_bytes seek=5120

stage1: build/stage1.bin

build/stage1.bin:
	$(ASM) src/stage1/boot.asm -f bin -o build/stage1.bin

stage2: build/stage2.bin

build/stage2.bin:
	$(ASM) src/stage2/main.asm -f bin -o build/stage2.bin

clean:
	rm -rf build/*
