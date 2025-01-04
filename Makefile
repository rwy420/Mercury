clean:
	cd kernel && make clean
	cd bootloader && make clean

image:
	cd kernel && make kernel.bin
	cp kernel/kernel.bin bootloader
	cd bootloader && make
	mv bootloader/build/image.img ./mercury_image.img
