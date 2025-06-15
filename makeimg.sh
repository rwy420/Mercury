#!/bin/bash
set -e

# Config
IMG=os.img
MNT=mnt
SIZE=512M

KERNEL=kernel/kernel.bin
GRUBCFG=grub/grub.cfg

# Clean old
sudo umount $MNT || true
rm -f $IMG

# Create raw image
dd if=/dev/zero of=$IMG bs=1 count=0 seek=$SIZE

# Create partition table with one FAT16 partition
parted $IMG --script mklabel msdos
parted $IMG --script mkpart primary fat16 1MiB 100%

# Setup loop device with partitions
LOOP=$(sudo losetup --find --partscan --show $IMG)
PART=${LOOP}p1

# Format partition as FAT16
sudo mkfs.fat -F 16 $PART

# Mount partition
mkdir -p $MNT
sudo mount $PART $MNT

# Install GRUB
sudo grub-install --target=i386-pc --boot-directory=$MNT/boot --modules="normal part_msdos fat multiboot" --no-floppy $LOOP

# Copy kernel + grub.cfg
sudo mkdir -p $MNT/boot/grub
sudo cp $KERNEL $MNT/boot/kernel.elf
sudo cp $GRUBCFG $MNT/boot/grub/grub.cfg

# Clean up
sudo umount $MNT
sudo losetup -d $LOOP

echo "✅ GRUB image created successfully!"

