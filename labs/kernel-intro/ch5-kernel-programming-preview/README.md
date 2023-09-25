
# Chapter 5 - Kernel Programming Preview

This chapter gives an introduction of:

- Error numbers and getting kernel output.
- Task structure.
- Memory allocation kmalloc/kfree, vmalloc/vfree, GFP_ATOMIC/GFP_KERNEL.
- Transferring data between user and kernel spaces.
- Linked lists
- String to number conversions
- Jiffies

## Error numbers and getting kernel output

The kernel function's return value can be checked by the macros defined in:

- https://elixir.bootlin.com/linux/v5.16.5/source/include/linux/err.h#L24

The Linux Kernel allows to encode the error in the return value instead of just
returning NULL.

```c
void *ERR_PTR (long error);
long IS_ERR (const void *ptr);
long PTR_ERR (const void *ptr);
long IS_ERR_OR_NULL(const void *ptr)
```

To print messages in the kernel, we can use `printk` and a macro that indicates
the severity.

- https://elixir.bootlin.com/linux/v5.16.5/source/include/linux/kern_levels.h#L8
- https://elixir.bootlin.com/linux/v5.16.5/source/include/linux/printk.h#L450


```c
#define KERN_SOH	"\001"		/* ASCII Start Of Header */
#define KERN_SOH_ASCII	'\001'

#define KERN_EMERG	KERN_SOH "0"	/* system is unusable */
#define KERN_ALERT	KERN_SOH "1"	/* action must be taken immediately */
#define KERN_CRIT	KERN_SOH "2"	/* critical conditions */
#define KERN_ERR	KERN_SOH "3"	/* error conditions */
#define KERN_WARNING	KERN_SOH "4"	/* warning conditions */
#define KERN_NOTICE	KERN_SOH "5"	/* normal but significant condition */
#define KERN_INFO	KERN_SOH "6"	/* informational */
#define KERN_DEBUG	KERN_SOH "7"	/* debug-level messages */

#define KERN_DEFAULT	""		/* the default kernel loglevel */
```

To understand the `"\001"`, we can see this program:


```c
/* Illustrate \OOO escape sequence */

#include <stdio.h>

int main(void)
{
    /* In the \000 escape sequence, the zeroes can be replaced by
     * octal numbers (0-7). Then, if we want to print the ASCII letter
     * 'A', we use \101, or for 'B', we use \102.
     */
    char* str = "Letter [\101] and [\102]\n";
    printf("%s", str);

    return 0;
}
````

## Task structure

Every process in Linux is represented by the `struct task_struct` defined
in `include/linux/sched.h`.

There is a special pointer called `current` that points to the current running
process. `current->tgid` yields to the ID of the current process and
`current->pid` yields to the task ID, When `tgid != pid`, it means that
the task is a thread of the process with `tgid == pid`.


## Memory allocation

There are several functions to allocate memory:


```c
#include <linux/slab.h>
void *kmalloc (size_t size, GFP_KERNEL);
void kfree (const void *addr);

#include <linux/mm.h>
unsigned long __get_free_pages (GFP_KERNEL, unsigned long order);
void free_pages (unsigned long addr, unsigned long order);

#include <linux/vmalloc.h>
void *vmalloc(unsigned long size) __alloc_size(1);
void vfree(const void *addr);
```

## Transferring data between user and kernel space

`PAGE_OFFSET` is macro that represents the start of the Kernel space and the
end of the user space.

The recommended function to access to the user space from the kernel are:

```c
#include <linux/uaccess.h>

int get_user (lvalue, ptr);
int put_user (expr, ptr);
long copy_from_user (void *to, const void __user *from, unsigned long n);
long copy_to_user (void __user *to, const void *from, unsigned long n);
long strncpy_from_user (char *dst, const char __user *src, long count);
long strlen_user (const char __user *src);
long strnlen_user (const char __user *src, long n);
unsigned long clear_user (void __user *to, unsigned long n);
```

The return value of `copy_from_user` and `copy_to_user` is the number
of bytes that were not successfully copied.

For example, if we want to transfer `nbytes`, we do:

```c
bytes_transferred = nbytes - copy_to_user (ubuf, kbuf, nbytes);
```

## Linked list

The Linux kernel has its own implementation of list call `list_head` that is found
in `include/linux/list.h`

```c
struct list_head {
    struct list_head *next, *prev;
};
```

The common functions and macros used to manipulate lists are:

```c
#include <linux/list.h>

LIST_HEAD(list_head);

void list_add (struct list_head *new, struct list_head *head)
void list_add_tail (struct list_head *new, struct list_head *head)
void list_del (struct list_head *entry)
int list_empty (struct list_head *head)
void list_splice (struct list_head *list, struct list_head *head)

list_entry (ptr, type, member);
list_for_each (pos, head)
list_for_each_prev (pos, head)
list_for_each_safe (pos, n, head)
```

## String to number conversions

To convert a string to number, the Linux kernel offers the functions below:

```c
int kstrtoull (const char *s, unsigned int base, unsigned long long *res);
int kstrtoll (const char *s, unsigned int base, long long *res);
int kstrtoul (const char *s, unsigned int base, unsigned long *res)
int kstrtol (const char *s, unsigned int base, long *res)
int kstrtouint(const char *s, unsigned int base, unsigned int *res);
int kstrtoint (const char *s, unsigned int base, int *res);
int kstrtou64 (const char *s, unsigned int base, u64 *res)
int kstrtos64 (const char *s, unsigned int base, s64 *res)
int kstrtou32 (const char *s, unsigned int base, u32 *res)
int kstrtos32 (const char *s, unsigned int base, s32 *res)
int kstrtou16 (const char *s, unsigned int base, u16 *res);
int kstrtos16 (const char *s, unsigned int base, s16 *res);
int kstrtou8 (const char *s, unsigned int base, u8 *res);
int kstrtos8 (const char *s, unsigned int base, s8 *res);
```

For strings passed from user-space we have:

```c
int kstrtoull_from_user (const char __user *s, size_t count, unsigned int base, unsigned long long *res);
int kstrtoll_from_user (const char __user *s, size_t count, unsigned int base, long long *res);
int kstrtoul_from_user (const char __user *s, size_t count, unsigned int base, unsigned long *res);
int kstrtol_from_user (const char __user *s, size_t count, unsigned int base, long *res);
int kstrtouint_from_user(const char __user *s, size_t count, unsigned int base, unsigned int *res);
int kstrtoint_from_user (const char __user *s, size_t count, unsigned int base, int *res);
int kstrtou16_from_user (const char __user *s, size_t count, unsigned int base, u16 *res);
int kstrtos16_from_user (const char __user *s, size_t count, unsigned int base, s16 *res);
int kstrtou8_from_user (const char __user *s, size_t count, unsigned int base, u8 *res);
int kstrtos8_from_user (const char __user *s, size_t count, unsigned int base, s8 *res);
int kstrtou64_from_user (const char __user *s, size_t count, unsigned int base, u64 *res);
int kstrtos64_from_user (const char __user *s, size_t count, unsigned int base, s64 *res);
int kstrtou32_from_user (const char __user *s, size_t count, unsigned int base, u32 *res);
int kstrtos32_from_user (const char __user *s, size_t count, unsigned int base, s32 *res);
```

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
```

## stress utility

`stress` utility allows to exercise certain kernel modules. For example:

```shell
$ stress -c 9 -i 7 -m 8 -t 30s
```
will:

- Fork off 9 CPU-intensive processes, each spinning on a sqrt() calculation.
- Fork off 7 I/O-intensive processes, each spinning on sync().
- Fork off 8 memory-intensive processes, each spinning on malloc(), allocating 256 MB by default.
  The size can be changed as in --vm-bytes 128M.
- Run the stress test for 30 seconds.


