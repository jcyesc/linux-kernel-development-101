
obj-m += arm64_info_mod.o

arm64_info_mod-y += arm64_mod.o
arm64_info_mod-y += arm64_mm.o
arm64_info_mod-y += arm64_cache.o

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
