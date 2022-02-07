
# Chapter 21 - Process Address Space


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


## Locking Pages

A process has the capability to lock its pages in order to avoid page swaps. A
normal user can lock only 16 pages and this can be configured by `ulimit`.


## Page faults

There are 3 ways that a process can get a `page fault`:

- When the requested page is valid for the process but it hasn't been allocated.

- When the page has been swapped out to allocate memory for other processes.

- When the process tries to access an invalid memory area.

