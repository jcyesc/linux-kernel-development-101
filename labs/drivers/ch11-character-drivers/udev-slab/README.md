
# udev and slab driver

To build and deploy the driver execute:

```shell
$ sudo insmod udev_slab_driver.ko 
$ ls -l /dev/udevslab101 
$ cat /proc/devices
$ sudo chmod 666 /dev/udevslab101
$ dmesg
[  205.251292] udevslabclass udevslab101: udev_device was registered successfully.
[  205.251308] udevslabclass udevslab101: MAJOR=240, MINOR=0
```

To build and run the user-space app:

```shell
$ arm-linux-gnueabi-gcc app_slab.c -o exec
$ scp exec pi@192.168.2.2:~
```

After executing the user-space, we have:


```shell
pi@raspberrypi:~ $ ./exec 
/dev/udevslab101 is open. File descriptor [3].
Return code from write [26]
/dev/udevslab101 is open. File descriptor [4].
Return code from write [12]
/dev/udevslab101 is open. File descriptor [3] 
Return code from read [26], msg = [This is a welcome message]
/dev/udevslab101 is open. File descriptor [4] 
Return code from read [12], msg = [Bienvenidos]
/dev/udevslab101 is open. File descriptor [3] 
Return code from read [21], msg = [is a welcome message]
/dev/udevslab101 is open. File descriptor [4] 
Return code from read [7], msg = [enidos]
pi@raspberrypi:~ $ 
```

In the kernel we can see:

```shell
[  778.868696] udevslabclass udevslab101: generic_cdev_open(): Opening device udevslab101: MAJOR 240 MINOR 0
[  778.868726] udevslabclass udevslab101: module_refcount=1
[  778.868795] udevslabclass udevslab101: generic_cdev_open(): Opening device udevslab101: MAJOR 240 MINOR 0
[  778.868814] udevslabclass udevslab101: module_refcount=2
[  778.869427] udevslabclass udevslab101: generic_cdev_write(): Starts lbuf=26, *ppos=0
[  778.869562] udevslabclass udevslab101: generic_cdev_write(): Starts lbuf=12, *ppos=0
[  778.869631] udevslabclass udevslab101: generic_cdev_llseek(): offset=0
[  778.869657] udevslabclass udevslab101: generic_cdev_llseek(): offset=0
[  778.869729] udevslabclass udevslab101: generic_cdev_read(): Starts lbuf=26, *ppos=0, nbytes=26
[  778.869750] udevslabclass udevslab101: generic_cdev_read(): ret 26 MSG: This is a welcome message
[  778.869856] udevslabclass udevslab101: generic_cdev_read(): Starts lbuf=12, *ppos=0, nbytes=12
[  778.869877] udevslabclass udevslab101: generic_cdev_read(): ret 12 MSG: Bienvenidos
[  778.869937] udevslabclass udevslab101: generic_cdev_llseek(): offset=5
[  778.869962] udevslabclass udevslab101: generic_cdev_llseek(): offset=5
[  778.870024] udevslabclass udevslab101: generic_cdev_read(): Starts lbuf=26, *ppos=5, nbytes=26
[  778.870052] udevslabclass udevslab101: generic_cdev_read(): ret 21 MSG: This is a welcome message
[  778.870252] udevslabclass udevslab101: generic_cdev_read(): Starts lbuf=12, *ppos=5, nbytes=12
[  778.870280] udevslabclass udevslab101: generic_cdev_read(): ret 7 MSG: Bienvenidos
[  778.870368] udevslabclass udevslab101: generic_cdev_release(): Releasing device: udevslab101
[  778.870386] udevslabclass udevslab101: generic_cdev_release(): MSG: This is a welcome message
[  778.870428] udevslabclass udevslab101: generic_cdev_release(): Releasing device: udevslab101
[  778.870446] udevslabclass udevslab101: generic_cdev_release(): MSG: Bienvenidos
```

