
# Modifying the user's signal handler in a module

The steps to compile and execute are:


In the desktop, execute:

```shell
# Cross-ompile module.
$ make

# Upload module and user space program to Raspberry Pi
$ scp *.ko pi@192.168.2.2:~/Development
$ scp lab_handler_userspace.c pi@192.168.2.2:~/Development

```

In the Raspberry Pi execute:

```shell
$ ssh pi@192.168.2.2
$ gcc -o lab_handler_userspace lab_handler_userspace.c
$ ./lab_handler_userspace
Process 1063 has installed the handler for signal 2
waiting to be modified by lab_handler module.
waiting to be modified by lab_handler module.
^C
^C
^C
^C
waiting to be modified by lab_handler module.
^C
waiting to be modified by lab_handler module.
waiting to be modified by lab_handler module.
waiting to be modified by lab_handler module.
```

In another console execute:


```shell
$ ssh pi@192.168.2.2
$ sudo insmod lab_handler.ko pid=1063 signo=2
$ dmesg
[ 1462.728890] lab_handler: loading out-of-tree module taints kernel.
[ 1462.729829] Resetting process lab_handler_use[1063] signal handler for 2 to SIG_DFL
..
```

After the console is loaded, if you type Ctrl-c in the user's space program, the
program should finish.


