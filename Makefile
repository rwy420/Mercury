IMG := kernel.img
MNT := mnt
SIZE := 512M

KERNEL := kernel/kernel.elf
GRUBCFG := grub/grub.cfg

clean:
	cd kernel && make clean
	cd bootloader && make clean

kernel.elf:
	cd kernel && make kernel.elf

grub: kernel.elf
	./makeimg.sh
	
rebuild: kernel.elf
	@echo "Mounting and copying new kernel..."
	LOOP=$$(sudo losetup --find --partscan --show $(IMG)); \
	PART=$${LOOP}p1; \
	mkdir -p $(MNT); \
	sudo mount $${PART} $(MNT); \
	sudo cp $(KERNEL) $(MNT)/boot/kernel.elf; \
	sync; \
	sudo umount $(MNT); \
	sudo losetup -d $${LOOP}; \
	echo "Kernel updated in $(IMG)"


qemu:
	qemu-system-x86_64 -enable-kvm -cpu host -drive file=kernel.img,if=none,id=usbstick,format=raw -usb -device usb-ehci,id=ehci -device usb-storage,bus=ehci.0,drive=usbstick -device rtl8139,netdev=net0 -netdev user,id=net0 -m 2048M

bochs:
	bochs -f bochs_config

libc:
	gcc -m32 -fPIE -shared -o libc.so lib/stdio.c lib/process.c
