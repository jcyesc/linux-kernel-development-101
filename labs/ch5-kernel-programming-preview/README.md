
# Chapter 5 - Kernel Programming Preview

This chapter will show examples for processes, lists and kmalloc/kfree.


## include/linux/jiffies.h - jiffies

`jiffies` is a measure of time used by the kernel. `jiffies` is incremented
`HZ` times per second.

The value of `HZ` can be found by executing:

```shell
$ cat .config | grep HZ
CONFIG_NO_HZ_COMMON=y
# CONFIG_HZ_PERIODIC is not set
CONFIG_NO_HZ_IDLE=y
# CONFIG_NO_HZ_FULL is not set
CONFIG_NO_HZ=y
# CONFIG_HZ_100 is not set
# CONFIG_HZ_250 is not set
# CONFIG_HZ_300 is not set
CONFIG_HZ_1000=y
CONFIG_HZ=1000
$ cat .config | grep HZ
``1