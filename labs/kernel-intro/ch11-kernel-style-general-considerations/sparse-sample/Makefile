
obj-m += lab_sparse_corrected.o

# Assumes that the Linux Kernel source code is in:
KERNEL_DIR ?= $(HOME)/Development/lsf-kernel-dev/linux-kernel-internals-LFD420/linux


# Invoking sparse
#
# Invoke sparse by adding the `C=` option:
#
#     $ make C=2 .....
#
# when compiling modules or the kernel; A value of C=1 does just the specific
# module file; a value of C=2 does all files

# Compile the module.
all:
	make C=2 -C $(KERNEL_DIR) \
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
