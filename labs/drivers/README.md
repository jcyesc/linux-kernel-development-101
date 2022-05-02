
# Developing Embedded Linux Device Drivers - LFD5435

Contains information and labs to develop Linux Device Drivers.

- Chapter 1 - Introduction
- Chapter 2 - Preliminaries
- Chapter 3 - How to work in OSS Projects
- Chapter 4 - Cross-Development Toolchain
- Chapter 5 - Basic Target Development Board Setup
- Chapter 6 - Booting a Target Development Board over Ethernet
- Chapter 7 - Kernel Configuration, Compilation Booting
- Chapter 8 - Device Drivers
- Chapter 9 - Modules and Device Drivers
- Chapter 10 - Memory Management and Allocation
- Chapter 11 - Character and Miscellaneous Device Drivers
- Chapter 12 - Kernel Features
- Chapter 13 - Transferring between user and kernel space
- Chapter 14 - Platform Drivers
- Chapter 15 - Device Trees
- Chapter 16 - Interrupts and Exceptions
- Chapter 17 - Timing measurements
- Chapter 18 - Kernel timers

## Build drivers for Ubuntu

To build the drivers for Ubuntu, create a `Makefile` with the configuration
from below:

```
obj-m += sample_char_driver.o


KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build

all:
	make -C $(KERNEL_DIR) \
		M=$(PWD) modules

clean:
	make -C $(KERNEL_DIR) \
		M=$(PWD) clean
```