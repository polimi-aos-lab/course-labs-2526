FROM ubuntu:focal
ARG DEBIAN_FRONTEND=noninteractive
ARG __ARCH=amd64
ARG __ENV=minimal
ENV __BUILD_ARCH=${__ARCH}

RUN echo "rebuild n.5"
RUN apt-get update
RUN apt-get install -y wget git qemu-system qemu-utils python3 python3-pip \
        gcc libelf-dev libssl-dev bc flex bison vim bzip2 libncurses-dev cpio

# Download kernel
RUN mkdir -p /sources
WORKDIR /sources
RUN wget https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/snapshot/linux-5.16-rc1.tar.gz
RUN tar xvzf linux-5.16-rc1.tar.gz
RUN mv linux-5.16-rc1 linux
RUN wget https://github.com/mirror/busybox/archive/refs/tags/1_32_1.tar.gz -O busybox-1.32.1.tar.gz
RUN tar xvzf busybox-1.32.1.tar.gz
RUN mv busybox-1_32_1 busybox-1.32.1

# initial build, so as to speed up development
RUN echo "Build n. 6"
COPY ./scripts/bootstrap.sh /sources
COPY ./scripts/.config-${__ARCH} /sources/linux/.config
COPY ./scripts/init /sources
RUN  /sources/bootstrap.sh

EXPOSE 5900
EXPOSE 6000

# Setting up a few tools for uefi demos
RUN apt-get install -y parted
RUN apt-get install -y dosfstools
RUN apt-get install -y gdb
RUN echo "Build n. 7"
COPY ./.dev-config/nvim-post-setup-${__ARCH}.sh /sources
COPY ./.dev-config/.tmux.conf /root
COPY ./.dev-config/.vimrc /root
RUN /bin/bash -c 'if [ "$__ENV" == "full" ]; then source /sources/nvim-post-setup-${__ARCH}.sh; fi'

EXPOSE 5550
COPY ./scripts/mini-bootstrap.sh /sources
