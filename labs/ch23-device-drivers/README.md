
# Chapter 23 - Device drivers

There are 3 types of device :


- Character device drivers: They handle hardware devices that stream bytes such
serial devices, parallel devices, etc. The basic operations of these drivers
are: `open`, `close`, `read` and `write`.

- Block device drivers: They handle block hardware devices such as hard disk,
ssd cards, etc. The transmission of data is done by blocks.

- Network device drivers: They handle hardware devices that transmit packets
of data, not stream or blocks of data. These drivers are mainly used to control
network cards.

## Device Nodes

In Linux, the way that user space communicate with devices is by using `device nodes`.
A device node is an entry in the filesystem, usually under `/dev/`, where user-space
programs can send requests to the device such as: `open`, `close`, `read` and `write`.

