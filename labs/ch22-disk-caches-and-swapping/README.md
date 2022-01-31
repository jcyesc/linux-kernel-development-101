
# Chapter 22 - Disk Caches and Swapping

In this chapter we discuss

- Caches
- Swapping
- Reverse Mapping
- OOM killer


## Caches

Linux uses different caches such as

- Hardware caches: You can see this information in:

```
$ cat /sys/devices/system/cpu/cpu2/cache/index0/size
32KB
```

- Software caches

- Page cache: Linux uses page caches in order to minimize the disk access due
it is expensive. The `sync` command flushes the modified memory into the block
device. The function `fsync()` flushes the modified memory into the block device.


## Swapping

Swapping is the process of moving data from memory to the disk and viceversa.
This is used to allow more process in memory.

Swapping is a bad thing because it prevents processes from hanging when free
memory is exhausted or it allows to launch a process even when there is not
more physical memory available.

We can find the `swap areas` using:

```
$ cat /proc/swaps
Filename				Type		Size	Used	Priority
/dev/sda5                               partition	1046524	0	-2
```

## Reverse mapping

The kernel memory manager includes a `reverse mapping (rmap)` algorithm. The
central idea is that when a page is to be freed one has to know more than the number
of tasks using the page; because it has to update the page table entries for each
task it must be able to walk its way from the page structure to all the page table
entries that point to it.

Without reverse mapping the kernel has to examine every page it is considering
swapping out and then identify all processes currently using it, so it can update
the page-table entry for the page in that process.

## Out of Memory (OOM) Killer

Linux permits the system to overcommit memory, so that it can grant memory
requests that exceed the size of RAM plus swap area.


Linux kernel permits overcommission of memory only for user processes. For
kernel processes, the pages are not swappable and are always allocated at request
time.


The overcommision is set by `/proc/sys/vm/overcommit_memory`:

```shell
# 0: (default) Permit overcommission.
$ sudo sh -c 'echo 0 > /proc/sys/vm/overcommit_memory'
$ cat /proc/sys/vm/overcommit_memory
0

# 1: All memory requests are allowed to overcommit.
$ sudo sh -c 'echo 1 > /proc/sys/vm/overcommit_memory'
$ cat /proc/sys/vm/overcommit_memory
1

# 2: Turn off overcommission.
$ sudo sh -c 'echo 2 > /proc/sys/vm/overcommit_memory'
$ cat /proc/sys/vm/overcommit_memory
2
```
