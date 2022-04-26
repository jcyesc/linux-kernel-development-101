
# Chapter 6 - Kernel modules

A `module` is kernel component that can be loaded or unloaded. The module helps
to extend the kernel functionality and it has access to kernel libraries. When
the kernel module is loaded, it becomes part of the big monolitic Linux kernel.


A module:

- has to have the `MODULE_LICENSE()` macro to avoid tainting the kernel.
- needs to be specified the init and clean up functions using the macros: 
`module_init()` and `module_exit()`.


## How to compile a module

To compile a module, we need to have access to kernel source that has been
through a compilation stage or at least has been through a
`make modules_prepare`, as this is required to generate necessary
configuration and dependency information. In addition to this, we also need
the `.config` file.

The kernel source code is usually found in
`/lib/modules/$(uname -r)/build/`.

To compile a module, we also need a minimal `Makefile` that contains the
following line:

```
obj-m += my_module_name.o
```

a complete `Makefile` would look like this:

```
obj-m += my_basic_module.o

KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build

# Compile the module.
all:
	make -C $(KERNEL_DIR) \
		ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
		M=$(PWD) modules

clean:
	make -C $(KERNEL_DIR) \
		ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
		M=$(PWD) clean
```

If the module was splitted in multiple source files, we can add these lines
to our `Makefile`:

```
obj-m += my_basic_module.o
mods-objs := my_basic1.o my_basic2.o
```

If you are not crosscompiling the kernel and modules, you can install the
modules using:


```shell
$ sudo make -C /lib/modules/$(uname -r)/build M=$PWD modules install
```

The modules can be loaded and unloaded with the following commands:


```shell
# Load module
sudo insmod my_module.ko

# List loaded modules
lsmod

# Unload module
sudo rmmod my_module
```

## How to enable module usuage count


One of the tasks of the kernel is to keep track of how many times a module
is currently referenced by user-space processes. In order to enable this, we
need to populate the `owner` field in `file_operations`:

```c
struct file_operations fops = {
    .owner = THIS_MODULE,
    ...
}
```

# How to avoid tainting the kernel

To avoid tainting the kernel, all the module code has to have a license defined
in `include/linux/license.h`:


```c
/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __LICENSE_H
#define __LICENSE_H

static inline int license_is_gpl_compatible(const char *license)
{
	return (strcmp(license, "GPL") == 0
		|| strcmp(license, "GPL v2") == 0
		|| strcmp(license, "GPL and additional rights") == 0
		|| strcmp(license, "Dual BSD/GPL") == 0
		|| strcmp(license, "Dual MIT/GPL") == 0
		|| strcmp(license, "Dual MPL/GPL") == 0);
}

#endif
```

The license to be used in the module is declared in this way:

```c

MODULE_LICENSE("Dual BSD/GPL");
```

Any licence other than the ones in `include/linux/license.h` causes the
kernel to be `tainted`. A warning appears when the module is loaded.


## How to get information about modules


To find out the modules loaded in the system use:

```shell
pi@raspberrypi:~ $ cat /proc/modules | awk ' {print $1;} '
```

Use `modinfo` to get information about a module.

```shell
pi@raspberrypi:~ $ modinfo ipv6
filename:       /lib/modules/5.10.75-v8+/kernel/net/ipv6/ipv6.ko
alias:          net-pf-10
license:        GPL
description:    IPv6 protocol stack for Linux
author:         Cast of dozens
srcversion:     5FF6DE19B200D619A9D880B
depends:
intree:         Y
name:           ipv6
vermagic:       5.10.75-v8+ SMP preempt mod_unload modversions aarch64
parm:           disable:Disable IPv6 module such that it is non-functional (int)
parm:           disable_ipv6:Disable IPv6 on all interfaces (int)
parm:           autoconf:En


# Get all the module licenses in the system:
pi@raspberrypi:~ $ cat /proc/modules | awk ' {print $1;} ' | xargs modinfo  | grep license

```

## How to find the kernel symbols

```shell
sudo cat /proc/kallsyms
```

