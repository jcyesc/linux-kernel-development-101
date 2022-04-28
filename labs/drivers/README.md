
# Developing Embedded Linux Device Drivers - LFD5435

Contains information and labs to develop Linux Device Drivers.


- Chapter 11 - Character Drivers
- Chapter 14 - Platform Drivers


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