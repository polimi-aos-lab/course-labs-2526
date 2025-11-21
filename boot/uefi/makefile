UEFIPATH=/opt/homebrew/Cellar/qemu/9.1.1/share/qemu
USEUEFI=-drive if=pflash,format=raw,read-only=on,file=${UEFIPATH}/edk2-x86_64-code.fd \
				-drive if=pflash,format=raw,file=./nvram.fd \
				-boot menu=on \
				-m 2G

DOCK=docker run -ti --privileged -v `pwd`:/local  -w /local --rm lkp-amd64-full-priv

start-uefishell: 
	cp ${UEFIPATH}/edk2-i386-vars.fd ./nvram.fd
	qemu-system-x86_64 ${USEUEFI}

start-uefilinux: LinuxDisk
	cp ${UEFIPATH}/edk2-i386-vars.fd ./nvram.fd
	qemu-system-x86_64 -smp 4 ${USEUEFI} -drive format=raw,file=LinuxDisk

connect:
	 $(DOCK) /bin/bash

LinuxDisk: ./create-disk.sh ./write-fs.sh
	$(DOCK) /local/create-disk.sh
	$(DOCK) /local/write-fs.sh



