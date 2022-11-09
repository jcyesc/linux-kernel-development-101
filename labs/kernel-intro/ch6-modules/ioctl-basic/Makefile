obj-m += msg_ioctl.o


# Assumes that the Linux Kernel source code is in:
KERNEL_DIR ?= $(HOME)/Development/linux-kernel-development-101/linux

# When kbuild executes, the current directory is always the root of the kernel
# tree (the argument to “-C”) and therefore an absolute path is needed. $(src)
# provides the absolute path by pointing to the directory where the currently
# executing kbuild file is located.
#
# https://www.kernel.org/doc/html/latest/kbuild/modules.html#several-subdirectories
ccflags-y := -I$(src)

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
