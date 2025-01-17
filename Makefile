clean:
	cd kernel && make clean
	cd bootloader && make clean

image:
	cd kernel && make kernel.bin
	cp kernel/kernel.bin bootloader
	cd bootloader && make
	mv bootloader/build/image.img ./mercury_image.img

grub:
	cd kernel && make grub
	mkdir -p iso/boot/grub
	mv kernel/grub-kernel.bin iso/boot/kernel.bin
	cp grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o mercury.iso iso
