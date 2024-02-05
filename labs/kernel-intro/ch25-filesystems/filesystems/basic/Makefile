# How to build the driver
#
# make KERNEL_DIR=$PATH_LINUX/linux-stable KERNEL_VERSION_5_15_139=1
# make KERNEL_DIR=$PATH_LINUX/linux-stable KERNEL_VERSION_6_6_2=1

obj-m += basic_fs.o

KERNEL_VERSION_6_6_2 ?= 0
KERNEL_VERSION_5_15_139 ?= 0

ccflags-y := -DKERNEL_VERSION_6_6_2=$(KERNEL_VERSION_6_6_2)
ccflags-y += -DKERNEL_VERSION_5_15_139=$(KERNEL_VERSION_5_15_139)

# Assumes that the Linux Kernel source code is in:
KERNEL_DIR ?= $(HOME)/Development/linux-kernel-development-101/linux

# Compile the module.
all:
	make -C $(KERNEL_DIR) \
		ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
		M=$(PWD) modules

clean:
	make -C $(KERNEL_DIR) \
		ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
		M=$(PWD) clean

# This option is used when you have connected the Raspberry Pi to
# the host using an ethernet connection. SCP stands for secure copy.
deploy:
	scp *.ko pi@192.168.2.2:~

deployqemu:
	scp -P 8022 ./*.ko root@127.0.0.1:~
