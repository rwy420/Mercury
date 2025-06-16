clean:
	cd kernel && make clean
	cd bootloader && make clean

image:
	cd kernel && make kernel.elf

qemu-run:
	qemu-system-x86_64 -enable-kvm -cpu host -drive file=kernel.img,if=ide,format=raw -device rtl8139,netdev=net0 -netdev user,id=net0 

bochs-run:
	bochs -f bochs_config

libc:
	gcc -m32 -fPIE -shared -o libc.so lib/stdio.c lib/process.c
