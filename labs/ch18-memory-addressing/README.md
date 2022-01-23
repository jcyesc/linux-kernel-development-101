# Chapter 18 - Memory Addressing

Linux's Virtual Memory Management system is used to control all access to
memory, including page caching, swapping, file access, process isolation, etc.

# NUMA (Non-Uniform Memory Access)

This term refers to architectures where the cpus don't have access to the same
memory.

`SMP` is an opposite architecture in which all CPUs share the same memory bus
and all memory is local.

As the number of CPUs gros the `SMP` architecture faces certain challenges
because the access to the same memory has to by synchronized for the different
CPUs and this is the main cause of bottlenecks.

A central concept in `NUMA` is the `node`. A node is a block of CPUs and
memory physically on the same bus. For instance once could have a `NUMA`
machine consisting of a number of `nodes`, each containing two CPUs which share
a 4GB `RAM`.

## Kernel Samepage Merging (KSM)

There are certain situations in which several pages of memory shared the same
content. For example, when several virtual machines are running the same code.
For this cases, the Linux kernel offers `Kernel Samepage Merging (KSM``.

With KSM, the kernel uses copy-on-write when the program will write on memory,
otherwise, several programs, virtual machines can share the same page of memory.

This functionality has to be configured with the flag `CONFIG_KSM`.


```shell
$ cat /boot/config-$(uname -r)  | grep KSM
CONFIG_KSM=y
```