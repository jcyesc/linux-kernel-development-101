# How to debug the kernel

There are many ways to debug the kernel. We can use:

- `printk()` to print messages in the kernel. These messages can be
found by running the command `dmesg`.
- `dump_stack()` to print the call stack of the function that is
executed.

These options are very common. Another useful approach is to debug
the kernel using gdb.

The vscode IDE and this project [FlorentREvest/linux-kernel-vscode]
allows you to do that.

To install the debugger and configure vscode, follow the instructions
in [FlorentRevest/linux-kernel-vscode].


> Note: A kernel built for x86-64 works properly out of the box in vscode
and we are able to debug it. However, for kernels built for ARM, it might
not work out of the box due the vscode plugin is unable to create a rootfs
for ARM. To use a custom rootfs instead, use this patch:
>
> - 0001-vscode-Configure-ARM-architecture-and-custom-rootfs.patch




[FlorentRevest/linux-kernel-vscode]: https://github.com/FlorentRevest/linux-kernel-vscode
