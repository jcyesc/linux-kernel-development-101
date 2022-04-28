# Character driver

The steps to run the driver are:


```shell
# Install the driver in RPi
$ sudo insmod sample_char_driver.ko

# Check that the device is registered.
$ cat /proc/devices | grep samcdev101
500 samcdev101

$ lsmod
Module                  Size  Used by
sample_char_driver     16384  0
cmac                   16384  1
aes_arm64              16384  1
rfcomm                 53248  4
...


# Create the device node.
$ sudo mknod -m 666 /dev/samcdev101 c 500 0
$ echo Thundercats > /dev/samcdev101
$ cat /dev/samcdev101
Thundercats
$ echo -n  12345 > /dev/samcdev101
$ cat /dev/samcdev101
12345$

# Fill with 2 blocks. Blocks are usually 512 bytes.
$ dd if=/dev/random of=/dev/samcdev101 count=2
dd if=/dev/random of=/dev/samcdev101 count=2
2+0 records in
2+0 records out
1024 bytes (1.0 kB, 1.0 KiB) copied, 0.00117182 s, 874 kB/s

$ cat /dev/samcdev101
$ dd if=/dev/samcdev101 count=1

# Removing device driver and node.
$ sudo rmmod sample_char_driver
$ sudo rm -rf /dev/samcdev101
```

The kernel logs look like

```shell
$ dmesg
[  255.597378] Character driver samcdev101 was registered successfully!
[  285.793904] sam_cdev_open(): Opening device: samcdev101 with private_data = 2333
[  285.793932] sam_cdev_open(): Opening device: MAJOR 500 MINOR 0
[  285.794132] sam_cdev_write(): Starts lbuf=12, *ppos=0
[  285.794181] sam_cdev_release(): Releasing device: samcdev101 with private_data = 2333
[  299.019136] sam_cdev_open(): Opening device: samcdev101 with private_data = 2333
[  299.019164] sam_cdev_open(): Opening device: MAJOR 500 MINOR 0
[  299.019264] sam_cdev_read(): Starts lbuf=131072, *ppos=0, *num_of_char_written=12
[  299.019391] sam_cdev_read(): Starts lbuf=131072, *ppos=12, *num_of_char_written=12
[  299.019493] sam_cdev_release(): Releasing device: samcdev101 with private_data = 2333
[  312.997679] sam_cdev_open(): Opening device: samcdev101 with private_data = 2333
[  312.997707] sam_cdev_open(): Opening device: MAJOR 500 MINOR 0
[  312.997827] sam_cdev_write(): Starts lbuf=5, *ppos=0
[  312.997882] sam_cdev_release(): Releasing device: samcdev101 with private_data = 2333
[  329.350382] sam_cdev_open(): Opening device: samcdev101 with private_data = 2333
[  329.350409] sam_cdev_open(): Opening device: MAJOR 500 MINOR 0
[  329.350504] sam_cdev_read(): Starts lbuf=131072, *ppos=0, *num_of_char_written=5
[  329.350624] sam_cdev_read(): Starts lbuf=131072, *ppos=5, *num_of_char_written=5
[  329.350710] sam_cdev_release(): Releasing device: samcdev101 with private_data = 2333
[  339.708730] sam_cdev_open(): Opening device: samcdev101 with private_data = 2333
[  339.708758] sam_cdev_open(): Opening device: MAJOR 500 MINOR 0
[  339.708993] sam_cdev_write(): Starts lbuf=512, *ppos=0
[  339.709157] sam_cdev_write(): Starts lbuf=512, *ppos=512
[  339.709214] sam_cdev_release(): Releasing device: samcdev101 with private_data = 2333
[  349.802103] sam_cdev_open(): Opening device: samcdev101 with private_data = 2333
[  349.802131] sam_cdev_open(): Opening device: MAJOR 500 MINOR 0
[  349.802232] sam_cdev_read(): Starts lbuf=131072, *ppos=0, *num_of_char_written=512
[  349.802475] sam_cdev_read(): Starts lbuf=131072, *ppos=512, *num_of_char_written=512
[  349.802577] sam_cdev_release(): Releasing device: samcdev101 with private_data = 2333
[  359.399414] sam_cdev_open(): Opening device: samcdev101 with private_data = 2333
[  359.399442] sam_cdev_open(): Opening device: MAJOR 500 MINOR 0
[  359.400280] sam_cdev_read(): Starts lbuf=512, *ppos=0, *num_of_char_written=512
[  359.400515] sam_cdev_release(): Releasing device: samcdev101 with private_data = 2333
[  369.801774] Character driver samcdev101 was unregistered successfully
```
