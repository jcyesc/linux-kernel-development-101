
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

