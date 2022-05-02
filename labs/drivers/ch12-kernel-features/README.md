# Chapter 12 - Kernel Features

- Components of the Kernel
- User space vs Kernel space
- What are system calls?
- Available system calls
- Scheduling algorithms and task structures
- Process context

## Components of the kernel

### Process Management

- Create processes.
- Schedule processes.
- Lifecycle of the process.
- Inter Process Communication (IPC).

### Memory Management

- Create the memory maps for the Kernel.
- Configure virtual memory for processes.
- Allocate  and free memory assigned to processes.
- Configure DMA regions.

### Filesystems

- Build data structures that map data in the hard disks.
- Provide API to access the filesystem.
- Provide access control mechanisms to the filesystem

### Device Management

- Define data structures that represent hardware devices.
- Provide a mechanism to access and manipulate hardware devices.

### Network

- Kernel has to provide a framework to process the Network packets.
- Kernel has to deliver the packets to the right process.
- Process might need to sleep while waiting for a message.
- Kernel has to handle routing and address resolution issues.


## User-space vs Kernel space

An operating system usually supports 2 modes of operation at least:

- `user mode`: Execution environment with less priveleges where user
applications execute.
- `kernel mode`: Execution environment with priveleged access to most of
the hardware in the system.

In modern systems, there is a most priveleged mode named `trust zone` where
the kernel doesn't have access and it is used to provide cryptographic services,
initialize hardware modules and enable virtualization.

## What are system calls?

System calls is the mechanism offered by the kernel so the user-space apps can
communicate with the kernel to perform actions such as:

- open/read/write a file.
- set file permissions.
- access to the devices such as printers, etc.

## Available system calls

In the Linux kernel `5.17.5` version there are 451 system calls for the ARM
`aarch64` architecture:

- https://elixir.bootlin.com/linux/latest/source/arch/arm64/include/asm/unistd.h#L41

```c
#define __NR_compat_syscalls		451
#endif
```

All the system calls available for the ARM `aarch64` are defined in:

- https://elixir.bootlin.com/linux/latest/source/arch/arm64/include/asm/unistd.h
- https://elixir.bootlin.com/linux/latest/source/arch/arm64/include/asm/unistd.h

## Scheduling algorithms and task structures

There are several scheduling algorithms in the Linux kernel depending of the type
of processes: real time, interactive processes, batch processing, etc.

Linux defines the `struct task_struct` to represent heavy or light weight
processes:

- https://elixir.bootlin.com/linux/latest/source/include/linux/sched.h#L728

As per the Linux kernel `5.17.5` version, the default scheduing algorithm is
`CFS Completely Fair Scheduler`.

## Process context

The kernel is in `process context` when it is executing code in behalf of a
process. This happens when a process invokes a system call for example.

