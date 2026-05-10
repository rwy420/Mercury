#!/bin/bash
set -e

IMG=kernel.img
MNT=mnt
SIZE=512M

KERNEL=kernel/kernel.elf
GRUBCFG=grub/grub.cfg

sudo umount $MNT || true
rm -f $IMG

dd if=/dev/zero of=$IMG bs=1 count=0 seek=$SIZE

parted $IMG --script mklabel msdos
parted $IMG --script mkpart primary fat32 10MiB 100%

LOOP=$(sudo losetup --find --partscan --show $IMG)
PART=${LOOP}p1

sudo mkfs.fat -F 32 $PART

mkdir -p $MNT
sudo mount $PART $MNT

sudo grub-install --target=i386-pc --boot-directory=$MNT/boot --modules="normal part_msdos fat multiboot" --no-floppy $LOOP

sudo mkdir -p $MNT/boot/grub
sudo cp $KERNEL $MNT/boot/kernel.elf
sudo cp $GRUBCFG $MNT/boot/grub/grub.cfg
sudo mkdir -p $MNT/bin 
echo "This is a file in the FAT32 root directory" > $MNT/README

sudo umount $MNT
sudo losetup -d $LOOP

sudo chmod -R 777 kernel.img 

echo "GRUB image created successfully!"
