
# Chapter 21 - Process Address Space


- Allocation User Memory and Address Spaces
- Locking Pages
- Memory descriptors and regions
- Access rights
- Allocating and freeing memory regions
- Page faults

## Allocating Kernel and User memory

### Kernel requests
When the kernel request memory, it is served as soon as possible.
Kernel memory is never swapped out. Because the kernel trusts itself, there is no protection
against programming errors.

### Device drivers
When a `device resource` requests memory, the device is
responsible of releasing it, otherwise there will be memory leaks. Furthermore,
there is not garbage collection.

### User space
When allocating memory for use by user-space processes, the situation is quite
different:

- Requests are considered of lower priority and may be deferred. For instance,
when a process issues a `malloc()` call, a range of addresses will be assigned,
but the actual memory request will not be fulfilled until the process actually
tries to use the memory. In other words, memory requests are considered to be
`non-urgent`
- It cannot be assumed that the process has not made addressing errors. The kernel
must ensure that all memory references are valid and be prepared to deal with any
that are not.


## Memory region and address space


A `memory region` is a range of addresses that can be used by a process.

The `address space` associated with a process consists of all `memory regions`
assigned to the process. The addresses within a memory region are continuous,
but the linked regions may or many not be contiguous with each other.


```
                Address Space
                  mm_struct
                       |
         -------------------------------
         |             |               |
       vm_area-------vm_area---------vm_area
```

We can check the memory maps for the processes using


```shell
$ cat /proc/[pid]/maps

$ pmap -d [pid]
```

## Locking Pages

A process has the capability to lock its pages in order to avoid page swaps. A
normal user can lock only 16 pages and this can be configured by `ulimit`.

```c
#include <sys/mman.h>

int mlock (const void *addr, size_t len);
int munlock (const void *addr, size_t len);
int mlockall (int flags);
int munlockall (void);
```

The APIs to lock the `struct mm_struct` are below:

```c
#include <linux/mmap_lock>

void mmap_lock(struct mm_struct *mm);
void mmap_unlock(struct mm_struct *mm);
void mmap_read_lock(struct mm_struct *mm);
void mmap_read_unlock(struct mm_struct *mm);
void mmap_write_lock(struct mm_struct *mm);
void mmap_write_unlock(struct mm_struct *mm);
```

## Memory descriptors and regions

The address space of a process is defined by its `memory descriptor`.


```c
include/linux/mm types.h

struct mm_struct;
struct vm_area_struct;
````

## Access rights

In `struct vm_area_struct` we can find the permissions for the different
memory regions.

```
include/linux/mm.h

* vm_flags in vm_area_struct, see mm_types.h.
 * When changing, update also include/trace/events/mmflags.h
 */
#define VM_NONE		0x00000000

#define VM_READ		0x00000001	/* currently active flags */
#define VM_WRITE	0x00000002
#define VM_EXEC		0x00000004
#define VM_SHARED	0x00000008

/* mprotect() hardcodes VM_MAYREAD >> 4 == VM_READ, and so for r/w/x bits. */
#define VM_MAYREAD	0x00000010	/* limits for mprotect() etc */
#define VM_MAYWRITE	0x00000020
#define VM_MAYEXEC	0x00000040
#define VM_MAYSHARE	0x00000080

#define VM_GROWSDOWN	0x00000100	/* general info on the segment */
#define VM_UFFD_MISSING	0x00000200	/* missing pages tracking */
#define VM_PFNMAP	0x00000400	/* Page-ranges managed without "struct page", just pure PFN */
#define VM_UFFD_WP	0x00001000	/* wrprotect pages tracking */

#define VM_LOCKED	0x00002000
#define VM_IO           0x00004000	/* Memory mapped I/O or similar */

					/* Used by sys_madvise() */
#define VM_SEQ_READ	0x00008000	/* App will access data sequentially */
#define VM_RAND_READ	0x00010000	/* App will not benefit from clustered reads */

#define VM_DONTCOPY	0x00020000      /* Do not copy this vma on fork */
#define VM_DONTEXPAND	0x00040000	/* Cannot expand with mremap() */
#define VM_LOCKONFAULT	0x00080000	/* Lock the pages covered when they are faulted in */
#define VM_ACCOUNT	0x00100000	/* Is a VM accounted object */
#define VM_NORESERVE	0x00200000	/* should the VM suppress accounting */
#define VM_HUGETLB	0x00400000	/* Huge TLB Page VM */
#define VM_SYNC		0x00800000	/* Synchronous page faults */
#define VM_ARCH_1	0x01000000	/* Architecture-specific flag */
#define VM_WIPEONFORK	0x02000000	/* Wipe VMA contents in child. */
#define VM_DONTDUMP	0x04000000	/* Do not include in the core dump */

```

## Allocating and freeing memory regions

The main functions to allocate and free memory regions are below:


```c
#include <linux/mm.h>

unsigned long do_mmap (struct file *file,
                       unsigned long addr,
                       unsigned long len,
                       unsigned long prot,
                       unsigned long flag,
                       unsigned long offset);
int do_munmap (struct mm_struct *mm, unsigned long addr, size_t len)
```

## Page faults

There are 3 ways that a process can get a `page fault`:

- When the requested page is valid for the process but it hasn't been allocated.

- When the page has been swapped out to allocate memory for other processes.

- When the process tries to access an invalid memory area.

