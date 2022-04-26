
# Chapter 9 - Kernel Configuration and Compilation


To configure different options in the Linux kernel, we can use `menuconfig`:

```
$ cd linux/
$ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- menuconfig
# In the graphical interface, use escape and / to search for the feature
# you are interested. Example
[ESC] /CONFIG_IKCONFIG_PROC

# Check that your configuration is in .config.
$ more .config | grep CONFIG_IKCONFIG_PROC
CONFIG_IKCONFIG_PROC=y
```

> Note: The `CONFIG_IKCONFIG_PROC=y` option will allow us to see the `.config` selection
> in `/proc/config.gz`.

After the Kernel has been installed, run:

```
pi@raspberrypi:~ $ zcat /proc/config.gz | grep IKCONFIG
CONFIG_IKCONFIG=y
CONFIG_IKCONFIG_PROC=y
```

## IS_ENABLED(option) macro

The macro `IS_ENABLED(option)` checks if one configuration was set in the
kernel. The usage is:

```c
if (IS_ENABLED(CONFIG_OF_FLATTREE)) {
	do something here
}

```

The functionality is similar to:

```shell
$ cat .config | grep CONFIG_OF_FLATTREE
CONFIG_OF_FLATTREE=y
...

```

- https://elixir.bootlin.com/linux/v5.17.4/source/include/linux/kconfig.h#L73


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
$ make cscope

# Start searching using cscope.
$ cscope
```
