# Chapter 14 - Processes


Linux is an operating system that is:

- Multi-process
- Multi-user
- Multi-processor

In a multi-process operating system, an process compete for the resources below:


- CPU time
- Memory
- File and Filesystems
- Peripheral devices such as Network card, speakers, mic, serial and parallel
ports, etc.


The `scheduler` is one of the most important parts of the operating sysstem. Its
job is to coordinate the assigment of CPUs to the processes in a timely and orderly
manner.

Linux represents a process using the `task_struct`. The maximum number of threads
in a system is given by:

> **threads_max = (TOTAL_RAM) / (8 * THREAD_SIZE)**

`THREAD_SIZE` is equals to the stack size, which could be 4KB, 8KB or 16KB
depending on the architecture and kernel version.

We can check the max number of threads by examing `/proc`

```shell
$ cat /proc/sys/kernel/threads-max
63288
```

For historical reasons, by default the largest PID has been limited to a signed
`16-bit number`, or `32768`. It is possible to alter this value by changing
`/proc/sys/kernel/pid_max`, since it may be inadequate for larger servers.
On 64-bit architectures, one can increase pid_max to the full 32-bit value, `2G`,
while for 32-bit the limit remains at `32K`


```shell
$ uname -a
Linux parallels-vm 4.15.0-142-generic #146~16.04.1-Ubuntu SMP Tue Apr 13 09:27:15 UTC 2021 x86_64 x86_64 x86_64 GNU/Linux
$ cat /proc/sys/kernel/pid_max
32768
```

In the Raspberry 3 Model B+ with 1GB of memory, we have

```shell
pi@raspberrypi:~ $ uname -a
Linux raspberrypi 5.10.75-v8+ #1 SMP PREEMPT Fri Nov 12 10:46:30 PST 2021 aarch64 GNU/Linux
pi@raspberrypi:~ $ cat /proc/sys/kernel/threads-max
6750
pi@raspberrypi:~ $ cat /proc/sys/kernel/pid_max
32768
```

As proceses are created, they will eventuaully reach the `pid_max`. At that moment,
they will start again at `pid = 30`.

When the kernel suspends a thread because it has reached its timeslice or an
interrupt happens and replaces it with another thread, this is called
`context switching`.

The `task_struct` that represents a process is defined in `include/linux/sched.h`.

## Heavyweight and Lighweight processes

Linux has the ability to create Heavyweight or Lightweight proceses.

A `heavyweight process` has its own memory, stack, files and filesystems. When the
process is cloned, the new process will use the same memory as the parent. When
the new process or the parent try to write in the memory, a `copy-on-write`
will happen.

A `lightweight process` is created with `clone()`, similar to a `heavyweight process`,
however, the parameters are different. A lightweight process or `thread`, shares the
same parent's memory, file descriptors and filesystems. A `thread` has its own
stack.

