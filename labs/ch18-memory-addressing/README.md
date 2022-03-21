# Chapter 18 - Memory Addressing

The main topics

- Virtual Memory Management
- Systems with and without MMU and the TLB
- Memory Addresses
- High and Low Memory
- Memory zones
- Special device nodes
- NUM
- Paging
- Page Tables
- page structure
- Kernel Samepage Merging (KSM)

Linux's Virtual Memory Management system is used to control all access to
memory, including page caching, swapping, file access, process isolation, etc.


## Virtual Memory Management

Linux needs support of MMU in order to work properly. The Memory Management Unit (MMU)
is in charge of converting virtual address to physical addresses. Configuring the MMU
is architecture dependant.


## Systems with and without MMU and the TLB

Not all the systems have MMU, and there are several Linux's patches that allow
to support MMU-less systems.


## Memory addresses

There are several memory addresses in the Linux Kernel:

- User Virtual Addresses
- Physical Addresses
- Bus Addresses
- Kernel Logical Addresses
- Kernel Virtual Addresses

The address ranges for user-space memory and kernel space are:


| Address Range      | Used by        |
| ------------------ | -------------- |
| [0, PAGE_OFFSET)   | User process   |
| [PAGE_OFFSET, 4GB) | Kernel memory  |


## High and Low memory

- Low memory is the memory that has been already jmapped by the kernel.
- High memory refers to the memory in 32-bits systems that is above 896MB and it
hasn't been mapped.

## Memory Zones

| Zone        | Meaning        | 32-bit Usage    | 64-bit Usage  |
| ----------- | -------------- |-----------------|---------------|
| DMA         | 24-bit ISA DMA | 0-16 MB         | 0-16 MB       |
| DMA32       | 32-bit DMA     | 16MB - 4GB      | 16MB - 4GB    |
| Normal      | Low Memory     | 16MB - 896MB    | 4GB - All RAM |
| HIGH        | High Memory    | 896MB - All Ram | No such zone  |


## Special Device memory nodes

There are several device memory nodes in the Linux kernel.

- /dev/men
- /dev/kmem
- /dev/null
- /dev/port
- /dev/zero
- /dev/null
- /dev/random
- /dev/urandom
- /dev/kmsg


## NUMA (Non-Uniform Memory Access)

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

## Paging

The memory in the Linux kernel is divided in pages, which are memory regions
of size `PAGE_SIZE`.

```c
#define PAGE_SIZE (1UL << PAGE_SHIFT)
```

The page size in x86 and arm systems is usually 4KB or `PAGE_SHIFT` equals to 12.
We can find the page size by calling the function `getpagesize()`.


## Page tables

The page tables are the data structures that performs the mapping between physical
addresses and virtual addresses. In 64-bit systems there are 3 or 4 levels, which
are called:

- Page Global Directory (PGD)
- Page Upper Directory (PUD)
- Page Middle Directory (PMD)
- Page Table Entry (PTE)
- Page Offset

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
