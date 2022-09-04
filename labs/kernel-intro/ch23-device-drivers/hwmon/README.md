
# Hardware Monitor

The driver in `hwmon_101.c` shows an example of a dummy hardware monitor
for the temperature sensor.

See https://docs.kernel.org/hwmon/hwmon-kernel-api.html


To install and see the `hwmon` in action, run:

```shell
pi $ sudo insmod hwmon_101.ko

pi $ ls -1 /sys/bus/platform/devices/hwmon_101_dev/hwmon/hwmon2/
device
name
of_node
power
subsystem
temp1_input
temp2_input
temp3_input
temp4_input
temp5_input
temp6_input
temp7_input
temp8_input
uevent

pi $ grep . /sys/bus/platform/devices/hwmon_101_dev/hwmon/hwmon2/temp*
/sys/bus/platform/devices/hwmon_101_dev/hwmon/hwmon2/temp1_input:33981
/sys/bus/platform/devices/hwmon_101_dev/hwmon/hwmon2/temp2_input:-10000
/sys/bus/platform/devices/hwmon_101_dev/hwmon/hwmon2/temp3_input:42651
/sys/bus/platform/devices/hwmon_101_dev/hwmon/hwmon2/temp4_input:-10000
/sys/bus/platform/devices/hwmon_101_dev/hwmon/hwmon2/temp5_input:11249307
/sys/bus/platform/devices/hwmon_101_dev/hwmon/hwmon2/temp6_input:-9830
/sys/bus/platform/devices/hwmon_101_dev/hwmon/hwmon2/temp7_input:13468911
/sys/bus/platform/devices/hwmon_101_dev/hwmon/hwmon2/temp8_input:-9813

pit $ sudo rmmod hwmon_101

pi $ dmesg
[   52.639043] hwmon_101: loading out-of-tree module taints kernel.
[   52.639733] hwmon_101: hwmon_101_probe executing.
[   52.639757] hwmon_101: hwmon_101_is_visible executing [hwmon_type]=1, [channel]=0
[   52.639766] hwmon_101: hwmon_101_is_visible executing [hwmon_type]=1, [channel]=1
[   52.639786] hwmon_101: hwmon_101_is_visible executing [hwmon_type]=1, [channel]=2
[   52.639794] hwmon_101: hwmon_101_is_visible executing [hwmon_type]=1, [channel]=3
[   52.639803] hwmon_101: hwmon_101_is_visible executing [hwmon_type]=1, [channel]=4
[   52.639810] hwmon_101: hwmon_101_is_visible executing [hwmon_type]=1, [channel]=5
[   52.639819] hwmon_101: hwmon_101_is_visible executing [hwmon_type]=1, [channel]=6
[   52.639827] hwmon_101: hwmon_101_is_visible executing [hwmon_type]=1, [channel]=7
[   52.640032] hwmon_101_driver hwmon_101_dev: raspberry_sensors_registered: hwmon2
[  164.952309] hwmon_101: hwmon_101_read executing [channel]=0
[  164.952917] hwmon_101: hwmon_101_read executing [channel]=1
[  164.953235] hwmon_101: hwmon_101_read executing [channel]=2
[  164.953719] hwmon_101: hwmon_101_read executing [channel]=3
[  164.954078] hwmon_101: hwmon_101_read executing [channel]=4
[  164.954409] hwmon_101: hwmon_101_read executing [channel]=5
[  164.954765] hwmon_101: hwmon_101_read executing [channel]=6
[  164.955398] hwmon_101: hwmon_101_read executing [channel]=7
[  215.728146] hwmon_101: hwmon_101_remove: Removing
```
