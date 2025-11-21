#!/bin/sh
# Check here https://sick.codes/how-to-mount-devices-inside-docker-containers-losetup-loopback-iso-files/
rm -f LinuxDisk
qemu-img create -f raw LinuxDisk 112M
parted LinuxDisk mklabel gpt
parted LinuxDisk mkpart primary 1MiB 101MiB # n. 1 - vfat (ESP), leave 1MiB buffer at the beginning
parted LinuxDisk mkpart primary 101MiB 100% # n. 2 - up to last usable sector
parted LinuxDisk set 1 boot on
parted LinuxDisk print
losetup -P -f LinuxDisk # don't know, must exit and reenter
