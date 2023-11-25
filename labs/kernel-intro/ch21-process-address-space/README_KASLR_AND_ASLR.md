# Address Space Layout Randomization

A security feature that the Linux kernel offers is to randomize the kernel
and user address space.

## Kernel Address Space Layout Randomization (KASLR)

KASLR is enable by default in the 6.x kernels. When the kernel is started, the
PAGE_OFFSET is randomized within a certain address space range.

The kernel configurations that are set by default are

- `CONFIG_RANDOMIZE_BASE`: randomize the locatoin of `PAGE_OFFSET`
- `CONFIG_RELOCATABLE`: compiles  kernel with Position Independent Code (PIC)
option.

During kernel boot time, KASLR can be disabled by passing  `nokaslr` to
the `kernel command line`. For example:

```
 # Start kernel with KASLR turn off (nokaslr).
 $ sudo qemu-system-aarch64 \
     -m 2048 \
     -cpu cortex-a72 \
     -machine virt \
     -nographic -smp 1 \
     -hda <path to>/buildroot/output/images/rootfs.ext2 \
     -kernel <path to>/linux-stable/arch/arm64/boot/Image.gz \
     -append "console=ttyAMA0 root=/dev/vda nokaslr oops=panic panic_on_warn=1 panic=-1 debug earlyprintk=serial" \
     -device virtio-net-pci,netdev=eth0 \
     -netdev user,id=eth0,hostfwd=tcp::8022-:22
```

## Userspace Address Space Layout Randomization (ASLR)

In the userpace, we can also randomize the address space layout. The pseudo file
that controls it is `/proc/sys/kernel/randomize_va_space`. `randomize_va_space`
can have 3 possible values

- 0 Turn off ASLR
- 1 Turn on ASLR but don’t randomize the heap.
- 2 Turn on ASLR and randomize the heap.

```
 $ cat /proc/sys/kernel/randomize_va_space
   2

 # Turn off ASLR
 $ echo 0 > /proc/sys/kernel/randomize_va_space
```

When ASLR is turned off, we can see that different address space range is
choosen everytime that we run `ldd` or `LD_TRACE_LOADED_OBJECTS=1 <exec file>`.

```
 $ LD_TRACE_LOADED_OBJECTS=1 ls
	linux-vdso.so.1 (0x0000ffffbd496000)
	libresolv.so.2 => /lib64/libresolv.so.2 (0x0000ffffbd430000)
	libc.so.6 => /lib64/libc.so.6 (0x0000ffffbd290000)
	/lib/ld-linux-aarch64.so.1 (0x0000ffffbd459000)
 $ LD_TRACE_LOADED_OBJECTS=1 ls
	linux-vdso.so.1 (0x0000ffff89421000)
	libresolv.so.2 => /lib64/libresolv.so.2 (0x0000ffff893c0000)
	libc.so.6 => /lib64/libc.so.6 (0x0000ffff89220000)
	/lib/ld-linux-aarch64.so.1 (0x0000ffff893e4000)
 $ LD_TRACE_LOADED_OBJECTS=1 ls
	linux-vdso.so.1 (0x0000ffffa04f2000)
	libresolv.so.2 => /lib64/libresolv.so.2 (0x0000ffffa0490000)
	libc.so.6 => /lib64/libc.so.6 (0x0000ffffa02f0000)
	/lib/ld-linux-aarch64.so.1 (0x0000ffffa04b5000)
```

If we turn off ASLR, the program will run in the same address space every time.

```
 $ echo 0 > /proc/sys/kernel/randomize_va_space
 $ LD_TRACE_LOADED_OBJECTS=1 ls
	linux-vdso.so.1 (0x0000fffff7ffb000)
	libresolv.so.2 => /lib64/libresolv.so.2 (0x0000fffff7f90000)
	libc.so.6 => /lib64/libc.so.6 (0x0000fffff7df0000)
	/lib/ld-linux-aarch64.so.1 (0x0000fffff7fbe000)
 $ LD_TRACE_LOADED_OBJECTS=1 ls
	linux-vdso.so.1 (0x0000fffff7ffb000)
	libresolv.so.2 => /lib64/libresolv.so.2 (0x0000fffff7f90000)
	libc.so.6 => /lib64/libc.so.6 (0x0000fffff7df0000)
	/lib/ld-linux-aarch64.so.1 (0x0000fffff7fbe000)
 $ LD_TRACE_LOADED_OBJECTS=1 ls
	linux-vdso.so.1 (0x0000fffff7ffb000)
	libresolv.so.2 => /lib64/libresolv.so.2 (0x0000fffff7f90000)
	libc.so.6 => /lib64/libc.so.6 (0x0000fffff7df0000)
	/lib/ld-linux-aarch64.so.1 (0x0000fffff7fbe000)
```
