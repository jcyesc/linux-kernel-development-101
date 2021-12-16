
# Note: There is not a thunder_cats_module.c file. The file thunder_cats_module.o
# is a concatenation of the files in thunder_cats_module-y.
obj-m += thunder_cats_module.o

# When kbuild executes, the current directory is always the root of the kernel
# tree (the argument to “-C”) and therefore an absolute path is needed. $(src)
# provides the absolute path by pointing to the directory where the currently
# executing kbuild file is located.
#
# https://www.kernel.org/doc/html/latest/kbuild/modules.html#several-subdirectories
ccflags-y := -I$(src)/include

# https://www.kernel.org/doc/html/latest/kbuild/modules.html#several-subdirectories
thunder_cats_module-y := src/krefs_module.o
thunder_cats_module-y += src/thunder_cat.o

# Assumes that the Linux Kernel source code is in:
KERNEL_DIR ?= $(HOME)/Development/lsf-kernel-dev/linux-kernel-internals-LFD420/linux

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
