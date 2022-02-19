
# Chapter 20 - Memory Allocation

This chapter discuesses the different types of memory allocation. The list of
the topics is below:

- Requesting and Releasing Pages
- Buddy System
- Slabs and Cache Allocations
- Memory Polls
- kmalloc()
- vmalloc()
- bootmen()
- Memory defragmentation


## Requesting and Release pages

There are a couple of functions define in `/include/linux/gfp.h` and implemented
in `/mm/page_alloc.c`. GFP stands for Get Free Pages.

```
unsigned long __get_free_pages(gfp_t gfp_mask, unsigned int order)
unsigned long get_zeroed_page(gfp_t gfp_mask)
```

- https://elixir.bootlin.com/linux/latest/source/mm/page_alloc.c#L5431
- https://elixir.bootlin.com/linux/latest/source/include/linux/gfp.h#L622

The types of memory that can be requested are passed through `gfp_mask` mask.
See the list and descriptions in [gfp.h](https://elixir.bootlin.com/linux/latest/source/include/linux/gfp.h#L256).

The list of flags for the kernel 5.16.10 is below:

```
GFP_ATOMIC
GFP_KERNEL
GFP_KERNEL_ACCOUNT
GFP_NOWAIT
GFP_NOIO
GFP_NOFS
GFP_USER
GFP_DMA
GFP_DMA32
GFP_HIGHUSER
GFP_HIGHUSER_MOVABLE
GFP_SKIP_KASAN_POISON
GFP_TRANSHUGE_LIGHT
GFP_TRANSHUGE
```

`order` gives the number of pages (as a power of 2). The limit is 1024 pages,
or order = 10 (4 MB on x86).


## Buddy System

After requesting and freeing memory blocks for several times, we can see
`memory fragmentation`. This can cause that memory requests start failing
because there is not enough contiguous memory.

One way to avoid this would be to use a paging circuitry in the memory management
unit (MMU) in the CPU to remap non-contiguous free pages into contiguous
addresses. This is not done for two reasons. The first is that it will not work
for DMA memory requests, which bypass the paging circuitry. The second is that
doing so would require frequent modification of the page tables, which would
slow memory access considerably.

Linux uses the `Buddy System` to minimize this situation. The algoritms
works like this. The kernel mantains a list of block of pages with sizes of: 1, 2,
4, 8, 16, 32, 64, 128, 256, 512, 1024. The max number of pages that a block
can have is 1024 (1024 * 4096 = 4MB). Each group starts in an address that is
multiple of the block size.

When a request comes, the `Buddy System` searches a block that is the closest
to the memory requested, if it finds it, it returns that address, otherwise, it
continues searching for the next bigger block, till it finds it. If it reaches
the 1024 block and it doesn't find a free block, the request fail.

When the pages are freed, the algorithm tries to merge the free page with a
`buddy` page. This means that the pages have to have the same size and the
first address of the block is a multiple of new merged block. Once that the pages
are merged, the algorithm tries to merge the new block with other contiguos block.
This algorithm keeps going till there are no more blocks to merge.


To see the list of free blocks per group, execute:

```
$ cat /proc/buddyinfo
Node 0, zone      DMA      1      0      1      0      2      1      1      0      1      1      3
Node 0, zone    DMA32    934   1211   1367   1406   1264   1096    920    552    305    129    337
Node 0, zone   Normal    142     37     60  23667  13519   3446    130      0      0      0      0
```

## Slabs and cache allocations

There are times when you need to allocate less than a page of memory for an
object. If you have to continually create and destroy objects, it would require
a lot of work from the kernel to get and release the memory. Linux provides a
solution for this: `the slab allocator`.

The slab allocator is a cache that contains your objects and keeps track of which
objects are free/used. This helps you to make better use of the memory.

To find out information about the `slab allocator` run:

```
$  sudo slabtop -o
 Active / Total Objects (% used)    : 730247 / 738798 (98.8%)
 Active / Total Slabs (% used)      : 18680 / 18680 (100.0%)
 Active / Total Caches (% used)     : 82 / 108 (75.9%)
 Active / Total Size (% used)       : 303656.91K / 305836.20K (99.3%)
 Minimum / Average / Maximum Object : 0.01K / 0.41K / 8.00K

  OBJS ACTIVE  USE OBJ SIZE  SLABS OBJ/SLAB CACHE SIZE NAME
214242 214242 100%    0.19K   5101       42     40808K dentry
192647 192647 100%    1.07K   6643       29    212576K ext4_inode_cache
 85952  85952 100%    0.06K   1343       64      5372K kmalloc-64
 34710  34710 100%    0.10K    890       39      3560K buffer_head
 27768  27679  99%    0.20K    712       39      5696K vm_area_struct
 26250  26102  99%    0.13K    875       30      3500K kernfs_node_cache
 17536  16776  95%    0.06K    274       64      1096K pid
 14280  14280 100%    0.04K    140      102       560K ext4_extent_status
 13515  12540  92%    0.60K    255       53      8160K inode_cache
 11050  11050 100%    0.02K     65      170       260K lsm_file_cache
  9870   5901  59%    0.09K    235       42       940K kmalloc-96
  9706   9311  95%    0.09K    211       46       844K anon_vma
  8448   8240  97%    0.25K    264       32      2112K filp
  6656   5724  85%    0.03K     52      128       208K kmalloc-32
  6328   6328 100%    0.57K    226       28      3616K radix_tree_node
  6144   6144 100%    0.01K     12      512        48K kmalloc-8
  5376   5376 100%    0.02K     21      256        84K kmalloc-16
  5088   4905  96%    0.50K    159       32      2544K kmalloc-512
  3570   3570 100%    0.05K     42       85       168K ftrace_event_field
  3136   3136 100%    0.06K     49       64       196K ext4_io_end
  2772   2772 100%    0.19K     66       42       528K kmalloc-192
  1974   1974 100%    0.19K     47       42       376K cred_jar
  1952   1773  90%    1.00K     61       32      1952K kmalloc-1024
  1825   1825 100%    0.05K     25       73       100K mbcache
  1792   1792 100%    0.07K     32       56       128K Acpi-Operand
  1696   1495  88%    0.12K     53       32       212K kmalloc-128
  1692   1511  89%    0.67K     36       47      1152K proc_inode_cache
  1665   1665 100%    0.70K     37       45      1184K shmem_inode_cache
```

The steps to create and use a `slab or cache` are:

1. First create the `slab`. See [slab_common.c](https://elixir.bootlin.com/linux/latest/source/mm/slab_common.c#L385)

```c
#include <linux/slab.h>

/*
 * kmem_cache_create - Create a cache.
 * @name: A string which is used in /proc/slabinfo to identify this cache.
 * @size: The size of objects to be created in this cache.
 * @align: The required alignment for the objects.
 * @flags: SLAB flags
 * @ctor: A constructor for the objects.
 */
struct kmem_cache *kmem_cache_create (
    const char *name, size_t size,
    size_t align, unsigned long flags,
    void (*ctor)(void *)
);
int kmem_cache_shrink (struct kmem_cache *cache);
void kmem_cache_destroy (struct kmem_cache *cache);
```
2. Then request memory for your objects using the previously created `slab`.

```c
void *kmem_cache_alloc(struct kmem_cache *cache, gfp_t gfp_mask);
void *kmem_cache_zalloc(struct kmem_cache *cache, gfp_t gfp_mask);
void kmem_cache_free (struct kmem_cache *cache, void *);
```

The kernel will automaticall release slabs that are free if memory is required.


## Memory Pools

`Memory Pools` are similar to `slabs`, the main difference is that the memory
in the pool is never trimmed or freed by the kernel. Caution must be used because
the kernel could suffer memory starvation.

To create a memory pool use:

```c
#include <linux/mempool.h>

mempool_t *mempool_create (int min_nr, mempool_alloc_t *alloc_fn, mempool_free_t *free_fn,
    void *pool_data);
void mempool_destroy (mempool_t *pool);
mempool_t *mempool_create_page_pool (int min_nr, int order);
mempool_t *mempool_create_kmalloc_pool (int min_nr, size_t size);
mempool_t *mempool_create_slab_pool (int min_nr, struct kmem_cache *kc);
int mempool_resize(mempool_t *pool, int new_min_nr, gfp_t gfp_mask);
```

To use memory from the pool use:

```c
void *mempool_alloc (mempool_t *pool, gfp_t gfp_mask);
void mempool_free (void *element, mempool_t *pool);
```

## kmalloc

When the memory allocations are less than a page size or not page size multiples,
`kmalloc` is used.


```c
void *kmallc (size_t size, gfp_t flags);
void *kzalloc (size_t size, gfp_t flags);
void *kcalloc (size_t n, size_t size, gfp_t flags);
void *krealloc (const void *p, size_t new_size, gfp_t flags);
void *kmalloc_array(size_t n, size_t size, gfp_t flags)
void *kmemdup (const void *src, size_t len, gfp_t gfp);
void kree (void * address);
```

The maximum you can request with kmalloc() is 1024 pages (4 MB on x86).

The memory returned by kmalloc is contiguous.


## vmalloc

Whenever possible it is preferable to map a continuous range of memory addresses
into contiguous pagess; this makes better use of caches and speeds memory access
as it requires less frequent modification of page tables.


The memory allocation methods considered so far (`__get_free_pages()`,
`kmalloc()`, and `kmem_cache_alloc`) all result in contiguous memory.

However, there are two situations where non-contiguous memory may either be
acceptable, or required:
– The penalty may be unimportant if the memory only needs to be referenced
infrequently.
– The size of the memory region needed is just too large to get in a contiguous
fashion; i.e., greater than the 1024 pages (4 MB with 4 KB pages) possible with
__get_free_pages().

The functions to allocate non=continguous free memory are:

```c
#include <linux/vmalloc.h>

void *vmalloc (unsigned long size);
void vfree (void *ptr);
```

The `vmalloc` allocations can be seen with:

```shell
$ cat  /proc/vmallocinfo
```

Since the 4.12 kernel version, Linux introduced a new set of functions that first
tries to allocate memory using `kmalloc()`, if it fails, it will try to use
`vmalloc()`.

```c
void *kvmalloc(size_t size, gfp_t flags);
void *kvzalloc(size_t size, gfp_t flags);
```

## Early allocations and bootmem()

When it is necessary to allocate more than 1024 pages (4MB) of contiguous memory,
`__get_free_pages()` can not be used. However, we can allocate more than 1024
pages during booting time using the functions:

```
#include <linux/bootmem.h>

void *alloc_bootmem (unsigned long size);
void *alloc_bootmem_low (unsigned long size);
void *alloc_bootmem_pages (unsigned long size);
void *alloc_bootmem_low_pages (unsigned long size);
```

The memory allocated with the previous functions can't be released later.

## Memory Defragmentation

There is a memory defragmentor in the Linux kernel since 2.6.23 kernel. To use
it, set the kernel flag  `CONFIG_COMPACTION=y`.

The basic idea of memory defragmentation is of course quite similar to hard disk
defragmentation (which is generally unnecessary in Linux filesystems): move pages
of data from one physical page to another in some kind of iterative process until
large free memory blocks appear.

To force the defragmentation process, once must force the pages in memory to be
swapped to this. We can do this by running:

```c
int main(void) {
	while(malloc(getpagesize()));
}
```

or executing:

```shell
$ sync
$ echo 3 > /proc/sys/vm/drop_caches
```

