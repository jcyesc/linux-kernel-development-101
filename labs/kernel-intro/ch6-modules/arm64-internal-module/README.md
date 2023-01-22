
# Internal kernel modules

Internal kernel modules are compiled at the same time that the kernel is compiled.
An internal module can be configured as:

- bool: If the value is `Y`, the module will be compiled into the kernel and
        loaded. If the value is `N`, the module won't be compiled.
- int: The value can be an int and a range can be specified. For example:

```
config TTY_PRINTK_LEVEL
	depends on TTY_PRINTK
	int "ttyprintk log level (1-7)"
	range 1 7
	default "6"
```

- tristate: It can have the values `Y`, `N` and `M`. The `Y` and `N` values
        means the same as in `boot`. `M` means that the module will be compiled
        but it won't be part of the kernel binary. The module can be uploaded
        using `insmod`.


Examples of how to define an internal module are below:

- [labs/kernel-intro/ch6-modules/internal-module](https://github.com/jcyesc/linux-kernel-development-101/commit/b45eda0c2f783686e9fa181aded60a52068998a0)
- [raspbian-linux-kernel-dev/drivers/hellomodule/hellomodule](https://github.com/jcyesc/raspbian-linux-kernel-dev/commit/a275d67dd8fcc68a426dd18a88c9d03464bf3837)

## Goals

The goals are:

- Create an internal module that will be compiled at the same time that the kernel.
- Modify and create the Kconfig files for the internal module.
- Define several configuration flags that will be used by the module.
- Print ARM processor information.

The internal module lives in `kernel/drivers/char/hw/arm64.c`.

To enable the new module, there are 2 ways:

1. In `menuconfig`, go to `> Device Drivers > Character devices > ARM64 info chararcter device`
an enable the different configurations.

```shell
$ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- menuconfig
```
2. In `.config`, enable the configuration.


```shell
$ vim .config

#
# ARM64 info character device
#
CONFIG_ARM64_INFO=y
CONFIG_ARM64_MEMORY_INFO=y
CONFIG_ARM64_CACHE_INFO=y
CONFIG_ARM64_EXCEPTION_INFO=y
CONFIG_ARM64_SYSTEM_REG_INFO=y
# end of ARM64 info character device

```

After setting up the configuration, we need to build the kernel and update
the SD card with the new kernel. Once that the new kernel is running in RPI 3,
we can see in the kernel logs:

```shell
$ dmesg

[    1.760350] arm64info: arm64info_init() ARM64 info
[    1.760366] arm64info: arm64info_init() CONFIG_ARM64_MEMORY_INFO is set
[    1.760380] arm64info: arm64info_init() CONFIG_ARM64_CACHE_INFO is set
[    1.760393] arm64info: arm64info_init() CONFIG_ARM64_EXCEPTION_INFO is set
[    1.760407] arm64info: arm64info_init() CONFIG_ARM64_SYSTEM_REG_INFO is set

```

If the driver is set as a module `CONFIG_ARM64_INFO=m`, to load it, we
have to run:

```shell
sudo insmod /lib/modules/$(uname -r)/kernel/drivers/char/hw/arm64info.ko
```

