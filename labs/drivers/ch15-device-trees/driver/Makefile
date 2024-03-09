
ARCH=arm
CROSS_COMPILE := arm-linux-gnueabi-

# ARCH=arm64
# CROSS_COMPILE=aarch64-linux-gnu-

obj-m += tpu_hw_driver.o

# Assumes that the Linux Kernel source code is in:
KERNEL_DIR ?= $(HOME)/Development/linux-kernel-development-101/linux

# Compile the module.
all:
	make -C $(KERNEL_DIR) \
		ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) \
		M=$(PWD)

clean:
	make -C $(KERNEL_DIR) \
		ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) \
		M=$(PWD) clean

# This option is used when you have connected the Raspberry Pi to
# the host using an ethernet connection. SCP stands for secure copy.
deploy:
	scp *.ko pi@192.168.2.2:~

deployqemu:
	scp -P 8022 ./*.ko root@127.0.0.1:~