
obj-m += hello_world_mod.o

# Assumes that the Linux Kernel source code is in:
KERNEL_DIR ?= $(HOME)/Development/tools/linux-stable

# Compile the module.
all:
	make -C $(KERNEL_DIR) \
		ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
		M=$(PWD) modules

clean:
	make -C $(KERNEL_DIR) \
		ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
		M=$(PWD) clean

