
# Chapter 9 - Kernel Configuration and Compilation


To configure different options in the Linux kernel, we can use `menuconfig`:

```shell
$ cd linux/
$ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- menuconfig
# In the graphical interface, use escape and / to search for the feature
# you are interested. Example
[ESC] /CONFIG_IKCONFIG_PROC

# Check that your configuration is in .config.
$ more .config | grep CONFIG_IKCONFIG_PROC
CONFIG_IKCONFIG_PROC=y
```

> Note: The `CONFIG_IKCONFIG_PROC=y` option will allow us to see the `.config`
> file in `/proc/config.gz`.

After the Kernel has been installed, run:

```shell
pi@raspberrypi:~ $ zcat /proc/config.gz | grep IKCONFIG
CONFIG_IKCONFIG=y
CONFIG_IKCONFIG_PROC=y
```

IMPORTANT: if `CONFIG_IKCONFIG` is set as a module (`m`), then you need
to install the module.

```shell
$ cat .config | grep CONFIG_IKCONFIG
CONFIG_IKCONFIG=m
CONFIG_IKCONFIG_PROC=y

# Start the module
pi@raspberrypi:~ $ sudo modprobe configs
pi@raspberrypi:~ $ zcat /proc/config.gz
```
## include/generated/autoconf.h

Once the kernel is configured and built, several files are generated, among them
is `include/generated/autoconf.h`, which it is included in `<linux/kconfig.h>`.
The `include/generated/autoconf.h` file contains macros that refers to
the selected kernel configurations. For example:

```
/*
 * Automatically generated file; DO NOT EDIT.
 * Linux/arm64 6.6.14 Kernel Configuration
 */
#define CONFIG_COMMON_CLK_SCMI 1
#define CONFIG_HAVE_ARCH_SECCOMP_FILTER 1
#define CONFIG_SND_PROC_FS 1
#define CONFIG_VFIO_PCI_MMAP 1
#define CONFIG_SCSI_DMA 1
#define CONFIG_NETFILTER_FAMILY_BRIDGE 1
#define CONFIG_SENSORS_ARM_SCMI 1
#define CONFIG_SPI_QCOM_QSPI_MODULE 1
#define CONFIG_CC_HAS_SANCOV_TRACE_PC 1
#define CONFIG_DEFAULT_INIT ""
#define CONFIG_ARM64_PAGE_SHIFT 12
#define CONFIG_ARM_MEDIATEK_CPUFREQ_HW_MODULE 1
#define CONFIG_SOUNDWIRE_MODULE 1
```

This macros can be used in the source code to verify whether a feature is enabled
or not:

```
#if IS_ENABLED(CONFIG_SOUNDWIRE_MODULE)

do_something();

#endif
```

See:

- https://elixir.bootlin.com/linux/v5.17.4/source/include/linux/kconfig.h#L73

## IS_ENABLED(option) macro

The macro `IS_ENABLED(option)` checks if one configuration was set in the
kernel. The usage is:

```c
if (IS_ENABLED(CONFIG_OF_FLATTREE)) {
	do something here
}
```

The kernel configurations that were set are in:

```shell
$ cat .config | grep CONFIG_OF_FLATTREE
CONFIG_OF_FLATTREE=y
```

## dump_stack()

When you want to know the call stack of one kernel function, we can use
the function `dump_stack()`.


## Kernel browsers

There are a lot of kernel browsers, two popular ones are:

- Bootlin: https://elixir.bootlin.com/linux/latest/source
- cscope

Bootlin allows to navigate and search through the kernel source code using a
web browser. On the other hand `cscope` offers a command line alternative.

To install and use `cscope`, follow the steps below:

```shell
$ sudo apt install cscope

# Go where the kernel source code is:
$ cd kernel_src/
$ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- COMPILED_SOURCE=1 cscope


# Start searching using cscope. To exit type Ctrl + d
# To try another Find option Ctrl + f
$ cscope


Find this C symbol:
Find this global definition: task_struct
Find functions called by this function:
Find functions calling this function:
Find this text string:
Change this text string:
Find this egrep pattern:
Find this file:
Find files #including this file:
Find assignments to this symbol:
```
