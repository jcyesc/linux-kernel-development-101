
obj-m += mem_user_kernel_driver.o

# -g flag will allow objdump to intersperse the source code with the assembly.
# To see the object dump use:
#
# $ aarch64-linux-gnu-objdump -S -d oops_module.ko
ccflags-y += -g


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


