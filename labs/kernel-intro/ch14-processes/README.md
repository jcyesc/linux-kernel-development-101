# Chapter 14 - Processes


Linux is an operating system that is:

- Multi-process
- Multi-user
- Multi-processor

In a multi-process operating system, a process compete for the resources below:

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


```c
#define _GNU_SOURCE
#include <sched.h>

int clone (int (*fn) (void *arg), void *child_stack, int flags, void *arg);
```

The cloning flags are defined in
[include/uapi/linux/sched.h](https://elixir.bootlin.com/linux/latest/source/include/uapi/linux/sched.h).


```c
#define CSIGNAL		0x000000ff	/* signal mask to be sent at exit */
#define CLONE_VM	0x00000100	/* set if VM shared between processes */
#define CLONE_FS	0x00000200	/* set if fs info shared between processes */
#define CLONE_FILES	0x00000400	/* set if open files shared between processes */
#define CLONE_SIGHAND	0x00000800	/* set if signal handlers and blocked signals shared */
#define CLONE_PIDFD	0x00001000	/* set if a pidfd should be placed in parent */
#define CLONE_PTRACE	0x00002000	/* set if we want to let tracing continue on the child too */
#define CLONE_VFORK	0x00004000	/* set if the parent wants the child to wake it up on mm_release */
#define CLONE_PARENT	0x00008000	/* set if we want to have the same parent as the cloner */
#define CLONE_THREAD	0x00010000	/* Same thread group? */
#define CLONE_NEWNS	0x00020000	/* New mount namespace group */
#define CLONE_SYSVSEM	0x00040000	/* share system V SEM_UNDO semantics */
#define CLONE_SETTLS	0x00080000	/* create a new TLS for the child */
#define CLONE_PARENT_SETTID	0x00100000	/* set the TID in the parent */
#define CLONE_CHILD_CLEARTID	0x00200000	/* clear the TID in the child */
#define CLONE_DETACHED		0x00400000	/* Unused, ignored */
#define CLONE_UNTRACED		0x00800000	/* set if the tracing process can't force CLONE_PTRACE on this clone */
#define CLONE_CHILD_SETTID	0x01000000	/* set the TID in the child */
#define CLONE_NEWCGROUP		0x02000000	/* New cgroup namespace */
#define CLONE_NEWUTS		0x04000000	/* New utsname namespace */
#define CLONE_NEWIPC		0x08000000	/* New ipc namespace */
#define CLONE_NEWUSER		0x10000000	/* New user namespace */
#define CLONE_NEWPID		0x20000000	/* New pid namespace */
#define CLONE_NEWNET		0x40000000	/* New network namespace */
#define CLONE_IO		0x80000000	/* Clone io context */
```

## Create Kernel threads

Kernel threads of execution differ in many important ways from those that
operate on behalf of a process. For one thing they always operate in kernel mode.

The API to create kernel threads is below:


```c
#include <linux/kthread.h>
struct task_struct *kthread_run (
	int (*threadfn)(void *data), void *data, const char namefmt[], ...);
struct task_struct *kthread_create (
	int (*threadfn)(void *data), void *data, const char namefmt[], ...);
void kthread_bind (struct task_struct *k, unsigned int cpu);
int kthread_stop (struct task_struct *k);
int kthread_should_stop (void);
int wake_up_process (struct task_struct *tsk);
```

## Executing User-Space Processes From Within the Kernel


To execute user-space process from within the kernel, use the API:


```c
#include <linux/umh.h>

// Possible values for the wait parameter in call_usermodehelper().
#define UMH_NO_WAIT 0 /* don't wait at all */
#define UMH_WAIT_EXEC 1 /* wait for the exec, but not the process */
#define UMH_WAIT_PROC 2 /* wait for the process to complete */
#define UMH_KILLABLE 4 /* wait for EXEC/PROC killable */

int call_usermodehelper (char *program_path, char *argv[], char *envp[], int wait);
```

