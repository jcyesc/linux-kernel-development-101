
# Chapter 16 - Monitoring and debugging


There are several tools to monitor and debug the kernel.


## `strace`

This tool helps us to identify the system call and the time that
it takes. This, in turn, serves to identify possible bottle necks.

```shell
$ strace [options] myprogram.o [arguments]

```


## `sysctl`

It helps to read and tune kernel parameters.


```shell
$ sysctl -a
```

The kernel parameters can also be found under `/proc/sys`.


To update a parameter, you can use any of these commands:

```shell
$ sudo sh -c 'echo 1 > /proc/sys/net/ipv4/ip_forward'
$ sudo sysctl net.ipv4.ip_forward=1
```

The kernel parameters can also be configured in `/etc/sysctl.conf`.


## `SysRq`

It is a tool that uses `SysRq` keyboard combinations. It can be used to
reboot the system, dump stack traces, etc.


```shell
# Reboot the system immediately without syncing or mounting the disks.
$ sudo sh -c 'echo b > /proc/sysrq-trigger'

# See the output in:
$ tail -f /var/log/messages
```

## oops Messages

`oops messages` indicate that there was a problem with the kernel. You can
trigger `oops messages` by using:

- `WARM()`: trigger the oops mesage.
- `BUG()`: trigger the oops message and kill the process.
- `panic()`: can induce a system crash.


The system can be set up to panic on oops:

```shell
$ echo 1 > /proc/sys/kernel/panic_on_oops

# Print the value
$ cat /proc/sys/kernel/panic_on_oops
$ sysctl kernel.panic_on_oops
```
# Kernel debuggers

There are several kernel debuggers:

- gdb
- crash
- kdb and kgdb
- eBPF (extended Berkeley Packet Filter):
http://www.brendangregg.com/blog/2019-01-01/learn-ebpf-tracing.html
- debugfs: Alternative filesystem to `/proc` to debug. It is mounted using:

```shell
$mount -t debugfs none /sys/kernel/debug
```
