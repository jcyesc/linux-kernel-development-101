
# Kernel Configuration and Compilation


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
