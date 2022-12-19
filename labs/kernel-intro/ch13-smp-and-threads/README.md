
# Chapter 13 - SMP and Threads


SMP stands for Symmetric Multi Processing. SMP means that the CPUs in a system
share the same memory and I/O processing.

## Processor Affinity

Affinity refers when the Operating System forces a process or group of processes
to run in certain CPUs.

In Linux is possible to specify in which processor an application should run. We
can also specify which CPU can handle certain interrupts.

For example

```shell
$ taskset -c 3 ./get_cpu
The CPU executing this program is 3
```

If you try to set the affinity of the get_cpu program to another CPU, the PID
changes and it could change the CPU where the program is running:


```shell
$ taskset -pc 1 13252
```

where 13252 is the PID of get_cpu.


## CPUSETS

Cpusets offer a far more robust way of handling processor affinity. Instead of
just assigning a mask of allowable CPUs to a particular process, one can form a
whole group of processes that shares the same affinity.

```shell
$ sudo -i
$ mkdir /dev/cpuset
$ mount -t cgroup -o cpuset cpuset /dev/cpuset
$ ls -l /dev/cpuset

# Create a new cpu set
$ cd /dev/cpuset
$ mkdir newcpuset

$ echo 2-3 > newcpuset/cpuset.cpus

# Assign current cpuset to the shell.
$ echo $$ > newcpuset/tasks
$ cat newcpuset/tasks
35585
37789
```

where $$ = 35585 is the current shell pid, and the second number is the cat process


## Per-CPU variables


When we have SoC that has several CPUs, we can define variables per cpu. Typically,
Per-CPU data is stored in an array. Each item in the array corresponds to a possible
processor in the system. See the API below:


```c
#include <linux/percpu.h>

DEFINE_PER_CPU (type, cpuvar);
DEFINE_PER_CPU (long[3], cpuvar);


get_cpu_var (cpuvar);
put_cpu_var (cpuvar);
per_cpu (cpuvar, int cpu_id);

void *alloc_percpu (type);
void free_percpu (const void *);

DECLARE_PER_CPU (type, cpuvar);

EXPORT_PER_CPU_SYMBOL (cpuvar);
EXPORT_PER_CPU_SYMBOL_GPL (cpuvar);

```

To find out in which CPU your task is running, you can use:


```c
#define get_cpu() ({ preempt_disable(); smp_processor_id(); })
#define put_cpu() preempt_enable()
```

Kernel preemption is the only concern with per-CPU data. Kernel preemption poses
two problems, listed here:

- If your code is preempted and reschedules on another processor, the cpu variable
is no longer valid because it points to the wrong processor. In general, code cannot
sleep after obtaining the current processor.

- If another task preempts your code, it can concurrently access my_percpu on the
same processor, which is a race condition.

This can be avoided by calling `get_cpu()`, which disables kernel preemption. The
corresponding call to `put_cpu()` enables kernel preemption.

