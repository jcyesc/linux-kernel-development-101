
obj-m += arm64_conf.o

arm64_conf-y += arm64_conf_info.o
arm64_conf-y += arm64lib.o

# https://www.kernel.org/doc/html/latest/kbuild/modules.html#several-subdirectories
ccflags-y := -I$(src)

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
