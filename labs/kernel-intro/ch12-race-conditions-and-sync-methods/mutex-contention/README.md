
# Lab - Use of mutex

In this lab, a mutex variable will be exported by `export_mutex.ko` and
2 other modules (`grab_mutex1.ko`, `grab_mutex2.ko`) will try to get
the `mutex`.

To compile the modules use:

```shell
$ cd labs/ch12-race-conditions-and-sync-methods/mutex-contention
$ make
$ ls *.ko
export_mutex.ko  grab_mutex1.ko  grab_mutex2.ko

# scp the files to the Raspberry Pi 3
$ scp *.ko pi@192.168.2.2:~/Development
```

ssh to the RPi and install the modules below:

```shell
$ sudo insmod export_mutex.ko
$ sudo insmod grab_mutex1.ko
$ dmesg
```

In other console, ssh to the RPi and install the module below:

```shell
$ sudo insmod grab_mutex2.ko
$ dmesg
```