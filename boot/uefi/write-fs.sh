#!/bin/sh
mkfs.vfat -F 32 /dev/loop0p1
mkfs.ext4 /dev/loop0p2
mkdir -p /efi
mount /dev/loop0p1 /efi
cp uefi-kernel/bzImage.efi /efi/bootx64.efi
cp uefi-kernel/initrd.img /efi
umount /efi
losetup -D
