# Chapter 7 - Kernel Architecture II


## Processes, Threads and Tasks


A `process` is an executing program and associated resources such as memory
mappings, open files, signal handlers, etc.

A `thread` is a light way process in the sense that it shares the resources
of other processes (memory, open files, etc).


The kernel can be in different contexts:

- Process context: executing code on behalf of the process.
- Interrupt context: dispatching a hardware interrupt.


To find out in which context the kernel is, use the functions in
`include/linux/preempt.h`.

- https://elixir.bootlin.com/linux/v5.16.5/source/include/linux/preempt.h


## Kernel Preemption

Code running in user space can be preempted any time by the kernel. The kernel
can decide when it is time to assign the processor to another task.


A kernel that is not preemptible does not mean that it can't be interrupted. A
Not preemptible kernel can be interrupted in these cases:

- The process goes to sleep because it is blocked waiting for a resource or
simply decides to sleep.
- An interrupt arrives while interrupts are enabled.

Before kernel 2.6, the Linux kernel was not preemtible.

There are 3 preemption options available:

1. `No Force Preemption`, used generally in servers.
2. `Voluntary Kernel Preemption`, used generally in Desktops. Explicit preeption
points are inserted in the kernel code to reduce the latency.
3. `Preemptible Kernel`, all kernel code is preemptible unless preemption is
disabled in a code section.

```shell
linux $ cat .config | grep PREEMPT
# CONFIG_PREEMPT_NONE is not set
# CONFIG_PREEMPT_VOLUNTARY is not set
CONFIG_PREEMPT=y
CONFIG_PREEMPT_COUNT=y
CONFIG_PREEMPTION=y
CONFIG_PREEMPT_RCU=y
CONFIG_DEBUG_PREEMPT=y
CONFIG_PREEMPTIRQ_TRACEPOINTS=y
# CONFIG_PREEMPT_TRACER is not set
# CONFIG_PREEMPTIRQ_DELAY_TEST is not set
```

## How to find the kernel configuration


In Raspbian, we run the commands below:


```shell
$ uname -r
5.10.75-v8+
$ more /etc/os-release 
PRETTY_NAME="Raspbian GNU/Linux 10 (buster)"
NAME="Raspbian GNU/Linux"
VERSION_ID="10"
VERSION="10 (buster)"
VERSION_CODENAME=buster
ID=raspbian
ID_LIKE=debian
HOME_URL="http://www.raspbian.org/"
SUPPORT_URL="http://www.raspbian.org/RaspbianForums"
BUG_REPORT_URL="http://www.raspbian.org/RaspbianBugs"
```

The `kernel configuration` can be found in:


```shell
# Go to the location where the kernel was compiled.
$ pwd
/home/parallels/Development/lsf-kernel-dev/linux
$ grep PREEMP ./.config
# CONFIG_PREEMPT_NONE is not set
# CONFIG_PREEMPT_VOLUNTARY is not set
CONFIG_PREEMPT=y
CONFIG_PREEMPT_COUNT=y
CONFIG_PREEMPTION=y
CONFIG_PREEMPT_RCU=y
CONFIG_DEBUG_PREEMPT=y
CONFIG_PREEMPTIRQ_TRACEPOINTS=y
# CONFIG_PREEMPT_TRACER is not set
# CONFIG_PREEMPTIRQ_DELAY_TEST is not set
```

In Ubuntu, we can run the commands below:

```shell
$ more /etc/os-release 
NAME="Ubuntu"
VERSION="16.04.6 LTS (Xenial Xerus)"
ID=ubuntu
ID_LIKE=debian
PRETTY_NAME="Ubuntu 16.04.6 LTS"
VERSION_ID="16.04"
HOME_URL="http://www.ubuntu.com/"
SUPPORT_URL="http://help.ubuntu.com/"
BUG_REPORT_URL="http://bugs.launchpad.net/ubuntu/"
VERSION_CODENAME=xenial
UBUNTU_CODENAME=xenial

$ uname -r
4.15.0-142-generic
```

The `kernel configuration` can be found in:

```shell
$ more /boot/config-$(uname -r) | grep PREEMPT
CONFIG_PREEMPT_NOTIFIERS=y
# CONFIG_PREEMPT_NONE is not set
CONFIG_PREEMPT_VOLUNTARY=y
# CONFIG_PREEMPT is not set
# CONFIG_PREEMPTIRQ_EVENTS is not set
```

## How to find the processor information

There are several ways to find process information

```shell
$ nproc
4
$ cat /proc/cpuinfo
```
