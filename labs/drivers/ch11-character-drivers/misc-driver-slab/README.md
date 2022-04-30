
# How to test the misc_driver_slab.c

Steps to install the driver

```shell
$ sudo insmod misc_driver_slab.ko 
$ ls /dev/miscslab101 
$ sudo chmod 666 /dev/miscslab101 
 ```

Compile and upload the user-space app:


```shell
$ arm-linux-gnueabi-gcc app_slab.c -o exec
$ scp exec pi@192.168.2.2:~
```

After executing the user-space, we have:


```shell
./exec 
/dev/miscslab101 is open. File descriptor [3].
Return code from write [26]
/dev/miscslab101 is open. File descriptor [4].
Return code from write [12]
/dev/miscslab101 is open. File descriptor [3] 
Return code from read [26], msg = [This is a welcome message]
/dev/miscslab101 is open. File descriptor [4] 
Return code from read [12], msg = [Bienvenidos]
/dev/miscslab101 is open. File descriptor [3] 
Return code from read [21], msg = [is a welcome message]
/dev/miscslab101 is open. File descriptor [4] 
Return code from read [7], msg = [enidos]
```

In the kernel we can see:

```shell
[ 9472.844232] misc miscslab101: generic_cdev_open(): Opening device miscslab101: MAJOR 10 MINOR 58
[ 9472.844264] misc miscslab101: module_refcount=1
[ 9472.844334] misc miscslab101: generic_cdev_open(): Opening device miscslab101: MAJOR 10 MINOR 58
[ 9472.844352] misc miscslab101: module_refcount=2
[ 9472.844786] misc miscslab101: generic_cdev_write(): Starts lbuf=26, *ppos=0
[ 9472.844917] misc miscslab101: generic_cdev_write(): Starts lbuf=12, *ppos=0
[ 9472.844993] misc miscslab101: generic_cdev_llseek(): offset=0
[ 9472.845019] misc miscslab101: generic_cdev_llseek(): offset=0
[ 9472.845206] misc miscslab101: generic_cdev_read(): Starts lbuf=26, *ppos=0, nbytes=26
[ 9472.845230] misc miscslab101: generic_cdev_read(): ret 26 MSG: This is a welcome message
[ 9472.845334] misc miscslab101: generic_cdev_read(): Starts lbuf=12, *ppos=0, nbytes=12
[ 9472.845354] misc miscslab101: generic_cdev_read(): ret 12 MSG: Bienvenidos
[ 9472.845423] misc miscslab101: generic_cdev_llseek(): offset=5
[ 9472.845448] misc miscslab101: generic_cdev_llseek(): offset=5
[ 9472.845511] misc miscslab101: generic_cdev_read(): Starts lbuf=26, *ppos=5, nbytes=26
[ 9472.845531] misc miscslab101: generic_cdev_read(): ret 21 MSG: This is a welcome message
[ 9472.845644] misc miscslab101: generic_cdev_read(): Starts lbuf=12, *ppos=5, nbytes=12
[ 9472.845665] misc miscslab101: generic_cdev_read(): ret 7 MSG: Bienvenidos
[ 9472.845755] misc miscslab101: generic_cdev_release(): Releasing device: miscslab101
[ 9472.845774] misc miscslab101: generic_cdev_release(): MSG: This is a welcome message
[ 9472.845815] misc miscslab101: generic_cdev_release(): Releasing device: miscslab101
[ 9472.845837] misc miscslab101: generic_cdev_release(): MSG: Bienvenidos
```

