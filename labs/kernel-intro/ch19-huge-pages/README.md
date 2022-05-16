
# Chapter 19 - Huge Pages


- Huge Page Support
- libhugetlbfs
- Transparent Huge Pages


Huge Pages support in the Linux kernel allows us to use large pages (4 MB or
2 MB in size) instead of 4KB pages, which drops the memory overhead for storing
page tables.

Furthermore, number of misses encountered when accessing the
`translation lookaside buffer (TLB)`, which caches the virtual to physical
address translations, is greatly reduced.

To configure Huge Pages, these 2 flags have to be set up in the `.config` file:

```shell
$ cat /boot/config-$(uname -r)  | grep HUGETLB
CONFIG_ARCH_WANT_GENERAL_HUGETLB=y
CONFIG_CGROUP_HUGETLB=y
CONFIG_HUGETLBFS=y
CONFIG_HUGETLB_PAGE=y
```

To see if the hugetlbfs is configured, run:

```shell
$ cat /proc/filesystems | grep huge
nodev	hugetlbfs
```

To see statistis about the huge pages being used, see:

```shell
$ cat /proc/meminfo | grep Huge
AnonHugePages:         0 kB
ShmemHugePages:        0 kB
HugePages_Total:       0
HugePages_Free:        0
HugePages_Rsvd:        0
HugePages_Surp:        0
Hugepagesize:       2048 kB
```

To see and modify the number of huge pages available, one can read or modified
the `proc` filesystem.

```shell
$ cat /proc/sys/vm/nr_hugepages
0
# Makes 10 huge pages.
$ echo 10 > /proc/sys/vm/nr_hugepages
```

We can also see huge page information and set allocations in the `/sys`
filesystem:

```shell
$ ls -1 /sys/kernel/mm/hugepages/hugepages-2048kB/
free_hugepages
nr_hugepages
nr_hugepages_mempolicy
nr_overcommit_hugepages
resv_hugepages
surplus_hugepages
$ cat /sys/kernel/mm/hugepages/hugepages-2048kB/free_hugepages
0
$ cat /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
0
$ cat /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages_mempolicy
0
$ cat /sys/kernel/mm/hugepages/hugepages-2048kB/resv_hugepages
0
$ cat /sys/kernel/mm/hugepages/hugepages-2048kB/surplus_hugepages
0
```

It is a good idea to set up the huge pages during booting time, otherwise, due
to fragmentation, it might be difficult to get the huge pages later.

```
hu`gepagesz=64K hugepages=128 hugepagesz=16M hugepages=4
```

It is possible to use the huge pages by using memory-mapping and shared-memory APIs.


## libhugetlbfs


`libhugetlbfs` is a library hosted in `https://github.com/libhugetlbfs/libhugetlbfs`
that allows us to use huge pages easily. For example, to use huge pages in the
program foo we can run:


```shell
$ LD_PRELOAD=/usr/lib64/libhugetlbfs.so HUGETLB_MORECORE=yes ./foo

# or

$ export LD_PRELOAD=/usr/lib64/libhugetlbfs.so
$ export HUGETLB_MORECORE=yes
$ ./foo
```

To allocate memory from huge pages, you can also use the API:

```c
#include <hugetlbfs.h>
void *get_hugepage_region(size_t len, ghr_t flags);
void free_hugepage_region(void *ptr);
void *get_huge_pages(size_t len, ghr_t flags);
void free_huge_pages(void *ptr);
```

The main difference between `get_hugepage_region()` and `get_huge_pages` is
that the former doesn't require to be `hugepage-aligned`, on the other hand,
the second function it requires `len` to be `hugepage-aligned`.


## Transparent Huge Pages

The Linux kernel introduced in the version `2.6.38` transparent huge pages. This
functionality allows us to use huge pages without too much management or attention
from the user.

Support for Transparent Huge Pages can be added to the kernel by setting:

```
CONFIG_TRANSPARENT_HUGEPAGE=y
```

More information in:

- https://www.kernel.org/doc/html/latest/admin-guide/mm/transhuge.html

