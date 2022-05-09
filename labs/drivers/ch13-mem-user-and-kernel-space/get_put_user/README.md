
# How to test the mem_user_kernel_driver.c

Steps to install the driver

```shell
$ sudo insmod mem_user_kernel_driver.ko
$ ls /dev/memuserkernel101
$ sudo chmod 666 /dev/memuserkernel101
 ```

Compile and upload the user-space app:


```shell
$ arm-linux-gnueabi-gcc app_slab.c -o exec
$ scp exec pi@192.168.2.2:~
```

After executing the user-space, we have:


```shell
./exec
/dev/memuserkernel101: File descriptor [3].
Return code from write [26]
/dev/memuserkernel101: File descriptor [4].
Return code from write [12]
/dev/memuserkernel101: File descriptor [3]
Return code from read [26], msg = [This is a welcome message]
/dev/memuserkernel101: File descriptor [4]
Return code from read [12], msg = [Bienvenidos]
/dev/memuserkernel101: File descriptor [3]
Return code from read [21], msg = [is a welcome message]
/dev/memuserkernel101: File descriptor [4]
Return code from read [7], msg = [enidos]
```

In the kernel we can see:

```shell
[   40.854652] misc memuserkernel101: cdriver_init() succeedded in registering device memuserkernel101
[   61.740403] misc memuserkernel101: generic_cdev_open(): Opening device memuserkernel101: MAJOR 10 MINOR 58
[   61.740435] misc memuserkernel101: module_refcount=1
[   61.740507] misc memuserkernel101: generic_cdev_open(): Opening device memuserkernel101: MAJOR 10 MINOR 58
[   61.740527] misc memuserkernel101: module_refcount=2
[   61.740984] misc memuserkernel101: generic_cdev_write(): Starts lbuf=26, *ppos=0, bytes_to_write=26
[   61.741007] misc memuserkernel101: generic_cdev_write(): Ends nbytes=26, *ppos=26
[   61.741124] misc memuserkernel101: generic_cdev_write(): Starts lbuf=12, *ppos=0, bytes_to_write=12
[   61.741158] misc memuserkernel101: generic_cdev_write(): Ends nbytes=12, *ppos=12
[   61.741233] misc memuserkernel101: generic_cdev_llseek(): offset=0
[   61.741258] misc memuserkernel101: generic_cdev_llseek(): offset=0
[   61.741460] misc memuserkernel101: generic_cdev_read(): Starts lbuf=26, *ppos=0, nbytes=26
[   61.741488] misc memuserkernel101: generic_cdev_read(): Ends nbytes=26, *ppos=26
               , msg=This is a welcome message
[   61.741605] misc memuserkernel101: generic_cdev_read(): Starts lbuf=12, *ppos=0, nbytes=12
[   61.741626] misc memuserkernel101: generic_cdev_read(): Ends nbytes=12, *ppos=12
               , msg=Bienvenidos
[   61.741700] misc memuserkernel101: generic_cdev_llseek(): offset=5
[   61.741725] misc memuserkernel101: generic_cdev_llseek(): offset=5
[   61.741788] misc memuserkernel101: generic_cdev_read(): Starts lbuf=26, *ppos=5, nbytes=26
[   61.741809] misc memuserkernel101: generic_cdev_read(): Ends nbytes=21, *ppos=26
               , msg=This is a welcome message
[   61.741920] misc memuserkernel101: generic_cdev_read(): Starts lbuf=12, *ppos=5, nbytes=12
[   61.741940] misc memuserkernel101: generic_cdev_read(): Ends nbytes=7, *ppos=12
               , msg=Bienvenidos
[   61.742035] misc memuserkernel101: generic_cdev_release(): Releasing device: memuserkernel101
[   61.742054] misc memuserkernel101: generic_cdev_release(): MSG: This is a welcome message
[   61.742095] misc memuserkernel101: generic_cdev_release(): Releasing device: memuserkernel101
[   61.742113] misc memuserkernel101: generic_cdev_release(): MSG: Bienvenidos
```

