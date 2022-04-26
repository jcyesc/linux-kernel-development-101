
# Chapter 11 - Kernel Style and General Considerations

In this chapter, we discuss:

- Code style
- Kernel docs
- Making kernel patches
- Writing Portable code for different CPUs, 32/64-bit, Endianness
- Writing for SMP
- Power Management
- Keeping security in Mind
- `sparse` tool to find semantic issues in the code

## Code style

The recommended code style for the kernel are in:

https://www.kernel.org/doc/html/latest/process/coding-style.html

In the kernel source code, we can find a script to format the code:
`scripts/Lindent`.

```shell
$ ./scripts/Lindent file_to_format.c
```

### How to define multi-lines for macros

```shell
#define my_macro(x,y) \
do {                  \
    for (;;) {        \
        if (x>y)      \
            break;    \
        schedule();   \
    }                 \
} while (0)
```

## Kernel documentation

The kernel-doc format can be found in:
https://www.kernel.org/doc/html/latest/doc-guide/kernel-doc.html.

To extract documentation, we can use:

```shell
$ cd /usr/src/linux
$ scripts/kernel-doc -man -function list_add /include/linux/list.h > list_add.txt
$ less list_add.txt
```

## Generate patches



To create a patch, you can use these comands:

- `patch`
- `git diff`


### Using `patch`

```shell
$ cd /usr/src
# Get the difference between files in the dirs: linux and linux_hacked.
$ diff -Nur linux linux_hacked > hacked_patch
````
The options in `diff` means:

- `N` option says consider files present in one tree and not the other.
- `u` option specifies `unified` format.
- `r` option forces `diff` to recurse through subdirectories.

To apply the patch, use:

```shell
$ cd /usr/src/linux
$ patch -p1 < hacked_patch
```

### Using `git patch`

We can see the differences between our changes and the last commit by
using:

```shell
$ git diff
```

To get all commits since you branched off of the master’ branch, each produced
as a separate sequential patch, run:

```shell
$ git format-patch master
```

## sparse

`sparse` is a parsing and analysis tool that is used by the kernel developers
to find semantic issues in the code, such as missing `__user`, `__init` labels
in the code.

To install it, use:

```shell
$ git clone https://git.kernel.org/pub/scm/devel/sparse/sparse.git
$ cd sparse
$ make
$ sudo make PREFIX=/usr/local install
```

To analyze the kernel code run:

```shell
# Search for semantic errors in all the files.
$ make C=2 ....

# Search for semantic errors in the given module.
$ make C=1 ....
```

## Using `likely()` and `unlikely()`

To help the compiler to make performance improvements, you can use the macros
`likely()` and `unlikely()` in the code:


```c
if (unlikely(!test)){
.... do something 1 ....
}

if ((unlikely(!test2)){
.... do something 2 ....
}

```

When using these macros, it is a good idea to check and profile if the assumptions
are right. To do this, we can enable the kernel flag:

```
CONFIG_PROFILE_ANNOTATED_BRANCHES=y`
```

compile the kernel, and check the `sysfs` for the results:

```shell
$ cat /sys/kernel/debug/trace_stat/branch_annotated
```

When the kernel is configured to optimize for size `CONFIG_CC_OPTIMIZE_FOR_SIZE`,
which means passing `-Os` to `gcc` instead of `-O2`, the likely() and unlikely()
macros are disabled.


## Writing portable code, CPUs, 32/64bit, endianess

It is important not to make assumptions about the platform in which the kernel
code is going to run. We must use use the standard data types and libraries
to manipulate bits, or declare addresses:

- `offset_t` must be used to define address offsets.
- Don't make assumptions about `big-endian` and `little-endian` architectures. Use
the kernel defined libraries.
- `arch` is the directory where the platform-dependent code should be.

