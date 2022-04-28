
# Character driver that uses dynamic device node creation

Below are the steps to test the driver.


```shell
$ sudo insmod dynamic_node_driver.ko 
$ dmesg 
[ 9891.198565] cdriver_init(): Character driver dynamicnode101 was registered successfully!
[ 9891.198596] cdriver_init(): MAJOR 240 MINOR 0


# Check that the device is registered.
$ cat /proc/devices | grep dynamicnode101
240 dynamicnode101

# create the device node
$ sudo mknod -m 666 /dev/dynamicnode101 c 240 0

$ echo Thundercats > /dev/dynamicnode101
$ cat /dev/dynamicnode101
Thundercats
$ echo -n  12345 > /dev/dynamicnode101
$ cat /dev/dynamicnode101
12345$
```