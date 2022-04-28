# Character driver

The steps to run the driver are:


```shell
# Install the driver in RPi
$ sudo insmod char_driver.ko

# Check that the device is registered.
$ cat /proc/devices | grep chardev101
$ lsmod

# Create the device node.
$ sudo mknod -m 666 /dev/chardev101 c 500 0
$ echo Thundercats > /dev/chardev101
$ cat /dev/chardev101

# Fill with 2 blocks. Blocks are usually 512 bytes.
$ dd if=/dev/random of=/dev/chardev101 count=2
$ cat /dev/chardev101
$ dd if=/dev/chardev101 count=1

# Removing device driver and node.
$ sudo rmmod char_drive
$ sudo rm -rf /dev/chardev101
```

In the logs, you should see something like:

```shell
$ dmesg
[ 6142.499544] Character driver chardev101 was registered successfully!
[ 6153.836590] char_dev_open(): Opening device: chardev101
[ 6153.841906] char_dev_write(): Starts lbuf=435, *ppos=0
[ 6153.841935] char_dev_write(): Ends nbytes=435, *ppos=435
[ 6153.842201] char_dev_open(): Releasing device: chardev101
[ 6183.957295] char_dev_open(): Opening device: chardev101
[ 6183.957412] char_dev_read(): Starts lbuf=131072, *ppos=0
[ 6183.957468] char_dev_read(): Ends nbytes=8192, *ppos=8192
[ 6183.959762] char_dev_read(): Starts lbuf=131072, *ppos=8192
[ 6183.959785] char_dev_read(): The chardev101 reached the end of the file
[ 6183.961132] char_dev_release(): Releasing device: chardev101
[ 6274.654861] Character driver chardev101 was unregistered successfully
```

For more info:

- https://linux-kernel-labs.github.io/refs/heads/master/labs/device_drivers.html