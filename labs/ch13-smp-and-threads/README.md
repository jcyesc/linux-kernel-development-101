
# Chapter 13 - SMP and Threads

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

