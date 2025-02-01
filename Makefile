clean:
	cd kernel && make clean
	cd bootloader && make clean

setup: fs.bin
	cd scripts/mercuryfs && make toolkit
	mv scripts/mercuryfs/fs ./mercuryfs-toolkit
	./mercuryfs-toolkit mkroot
	./mercuryfs-toolkit wroot

fs.bin:
	dd if=/dev/zero of=fs.bin bs=1 count=687109

image:
	cd mercury && make mercury
	cd kernel && make kernel.bin
	cp kernel/kernel.bin bootloader
	cd bootloader && make
	mv bootloader/build/image.img ./build/mercury_image.img

vdi: image
	dd if=build/mercury_image.img of=build/disk.vdi conv=notrunc oflag=seek_bytes seek=2097152

grub:
	cd kernel && make grub
	mkdir -p iso/boot/grub
	mv kernel/grub-kernel.bin iso/boot/kernel.bin
	cp grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o ./build/mercury.iso iso
