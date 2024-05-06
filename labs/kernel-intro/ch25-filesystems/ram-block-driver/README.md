
# Ram block device driver

This page describes how to build the block driver,
deploy it and test it.

The block driver was tested in the Linux kernel version `6.6.2`.

## Build and deploy the block driver

To build the driver in the host and deploy it in the target, run:

```
 $ make clean
 $ make KERNEL_DIR=$HOME/Development/tools/linux-stable
 $ make deployqemu
 scp -P 8022 ./*.ko root@127.0.0.1:~
 ram_block_driver.ko
```

Once the driver is in the target, install the driver:

```
qemu $ mknod /dev/ramdiskblockdev b 240 0
qemu $ insmod ram_block_driver.ko
[   82.022617] ram_block_driver: loading out-of-tree module taints kernel.
[   82.040027] ram_block_driver: ram_block_init: Before register_blkdev()
[   82.040852] ram_block_driver: init_blk_mq_tag_set: Before blk_mq_alloc_tag_set()
[   82.041709] ram_block_driver: init_request_queue: Before blk_mq_init_queue()
[   82.042553] ram_block_driver: init_request_queue: Before blk_queue_physical_block_size()
[   82.043051] ram_block_driver: init_gendisk: Before blk_mq_alloc_disk()
[   82.046716] ram_block_driver: init_gendisk: Before set_capacity()
[   82.047345] ram_block_driver: setup_block_dev: Before add_disk()
[   82.069836] ram_block_driver: ram_block_open: Open disk genramdisk
[   82.074355] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request
[   82.074896] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[   82.074986] ram_block_driver: pr_request: READ direction request
[   82.075492] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  0
[   82.080463] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 4096
[   82.081221] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 4096
[   82.081641] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[   82.081773] ram_block_driver: exec_request: Processing segments in bio
[   82.082310] ram_block_driver: exec_request: READ Segment 0
[   82.083012] ram_block_driver: exec_request:    sector:    0
[   82.089588] ram_block_driver: exec_request:    bv_offset: 0
[   82.090164] ram_block_driver: exec_request:    offset:    0
[   82.090578] ram_block_driver: exec_request:    len:       4096
[   82.094212] ram_block_driver: ram_block_release: Release disk genramdisk
[   82.095423] ram_block_driver: ram_block_init: Make sure to create the node:
```

## Verify that the block device, partition and ramdisk are created

To verify that the partition was created, run:

```
 $ cat /proc/partitions
major minor  #blocks  name

 254        0     614400 vda
  31        0     131072 mtdblock0
 240        0      65536 genramdisk
```

To verify that the block device and ramdisk were created, run:

```
 $ cat /proc/devices | grep ramdiskblockdev
240 ramdiskblockdev

 $ ls -la /dev/genramdisk
 brw-------    1 root     root      240,   0 Dec 28 04:58 /dev/genramdisk
```

To see the configurations of the disk:

```
 $ for f in `find /sys/block/genramdisk/ -type f`; do echo $f; cat $f; done;
/sys/block/genramdisk/uevent
MAJOR=240
MINOR=0
DEVNAME=genramdisk
DEVTYPE=disk
DISKSEQ=11
/sys/block/genramdisk/ext_range
256
/sys/block/genramdisk/range
1
/sys/block/genramdisk/alignment_offset
0
/sys/block/genramdisk/diskseq
11
/sys/block/genramdisk/power/runtime_active_time
0
/sys/block/genramdisk/power/runtime_status
unsupported
/sys/block/genramdisk/power/autosuspend_delay_ms
cat: read error: Input/output error
/sys/block/genramdisk/power/runtime_suspended_time
0
/sys/block/genramdisk/power/control
auto
/sys/block/genramdisk/dev
240:0
/sys/block/genramdisk/ro
0
/sys/block/genramdisk/mq/0/cpu_list
0
/sys/block/genramdisk/mq/0/nr_reserved_tags
0
/sys/block/genramdisk/mq/0/nr_tags
16
/sys/block/genramdisk/stat
       1        0        8       17        0        0        0        0        0       20       17        0        0        0        0        0        0
/sys/block/genramdisk/events_poll_msecs
-1
/sys/block/genramdisk/events_async
/sys/block/genramdisk/queue/io_poll_delay
-1
/sys/block/genramdisk/queue/max_integrity_segments
0
/sys/block/genramdisk/queue/zoned
none
/sys/block/genramdisk/queue/scheduler
none [mq-deadline] kyber bfq
/sys/block/genramdisk/queue/io_poll
0
/sys/block/genramdisk/queue/discard_zeroes_data
0
/sys/block/genramdisk/queue/minimum_io_size
512
/sys/block/genramdisk/queue/nr_zones
0
/sys/block/genramdisk/queue/write_same_max_bytes
0
/sys/block/genramdisk/queue/max_segments
128
/sys/block/genramdisk/queue/dax
0
/sys/block/genramdisk/queue/dma_alignment
511
/sys/block/genramdisk/queue/physical_block_size
512
/sys/block/genramdisk/queue/logical_block_size
512
/sys/block/genramdisk/queue/virt_boundary_mask
0
/sys/block/genramdisk/queue/zone_append_max_bytes
0
/sys/block/genramdisk/queue/nr_requests
32
/sys/block/genramdisk/queue/write_cache
write through
/sys/block/genramdisk/queue/stable_writes
0
/sys/block/genramdisk/queue/max_segment_size
65536
/sys/block/genramdisk/queue/rotational
0
/sys/block/genramdisk/queue/discard_max_bytes
0
/sys/block/genramdisk/queue/add_random
0
/sys/block/genramdisk/queue/discard_max_hw_bytes
0
/sys/block/genramdisk/queue/optimal_io_size
0
/sys/block/genramdisk/queue/chunk_sectors
0
/sys/block/genramdisk/queue/iosched/front_merges
1
/sys/block/genramdisk/queue/iosched/read_expire
500
/sys/block/genramdisk/queue/iosched/prio_aging_expire
10000
/sys/block/genramdisk/queue/iosched/fifo_batch
16
/sys/block/genramdisk/queue/iosched/write_expire
5000
/sys/block/genramdisk/queue/iosched/writes_starved
2
/sys/block/genramdisk/queue/iosched/async_depth
6
/sys/block/genramdisk/queue/read_ahead_kb
128
/sys/block/genramdisk/queue/max_discard_segments
1
/sys/block/genramdisk/queue/write_zeroes_max_bytes
0
/sys/block/genramdisk/queue/nomerges
0
/sys/block/genramdisk/queue/zone_write_granularity
0
/sys/block/genramdisk/queue/fua
0
/sys/block/genramdisk/queue/discard_granularity
0
/sys/block/genramdisk/queue/rq_affinity
1
/sys/block/genramdisk/queue/max_sectors_kb
127
/sys/block/genramdisk/queue/hw_sector_size
512
/sys/block/genramdisk/queue/max_hw_sectors_kb
127
/sys/block/genramdisk/queue/iostats
1
/sys/block/genramdisk/size
256
/sys/block/genramdisk/integrity/write_generate
0
/sys/block/genramdisk/integrity/format
none
/sys/block/genramdisk/integrity/read_verify
0
/sys/block/genramdisk/integrity/tag_size
0
/sys/block/genramdisk/integrity/protection_interval_bytes
0
/sys/block/genramdisk/integrity/device_is_integrity_capable
0
/sys/block/genramdisk/discard_alignment
0
/sys/block/genramdisk/capability
[   82.095503] ram_block_driver: ram_block_init:     mknod /dev/ramdiskblockdev b 240 0
[  221.049371] block genramdisk: the capability attribute has been deprecated.
0
/sys/block/genramdisk/hidden
0
/sys/block/genramdisk/removable
0
/sys/block/genramdisk/events
/sys/block/genramdisk/inflight
       0        0
```

## Format the block device and mount a file system

1. Format the block device with ext4

```
# mke2fs  /dev/ramdiskblockdev
Filesystem label=
OS type: Linux
Block size=1024 (log=0)
Fragment size=1024 (log=0)
16384 inodes, 65536 blocks
3276 blocks (5%) reserved for the super user
First data block=1
Maximum filesystem blocks=262144
8 block groups
8192 blocks per group, 8192 fragments per group
2048 inodes per group
Superblock backups stored on blocks:
	8193, 24577, 40961, 57345

# file -s /dev/ramdiskblockdev
/dev/ramdiskblockdev: Linux rev 1.0 ext2 filesystem data, UUID=429564ef-6529-4021-985a-43ffc4ea8067

# file -s /dev/genramdisk
/dev/genramdisk: Linux rev 1.0 ext2 filesystem data, UUID=517a7e2c-6eea-420f-b104-8ea87c84173a (large files)
```

Alternatively, you could also format `/dev/genramdisk` instead.


```
# mke2fs /dev/genramdisk
mke2fs 1.47.0 (5-Feb-2023)
Creating filesystem with 65536 1k blocks and 16384 inodes
Filesystem UUID: 517a7e2c-6eea-420f-b104-8ea87c84173a
Superblock backups stored on blocks:
	8193, 24577, 40961, 57345

Allocating group tables: done
Writing inode tables: done
Writing superblocks and filesystem accounting information: done


# file -s /dev/ramdiskblockdev
/dev/ramdiskblockdev: Linux rev 1.0 ext2 filesystem data, UUID=517a7e2c-6eea-420f-b104-8ea87c84173a (large files)
# file -s /dev/genramdisk
/dev/genramdisk: Linux rev 1.0 ext2 filesystem data, UUID=517a7e2c-6eea-420f-b104-8ea87c84173a (large files)
```

> IMPORTANT: In this example /dev/ramdiskblockdev and /dev/genramdisk refers
> to the same partition. However, if we would have more partitions, the structure
> would be:
>
>    /dev/ramdiskblockdev
>    /dev/genramdisk0
>    /dev/genramdisk1
>    /dev/genramdisk2
>
> Similar to:
>
>    /dev/sda
>    /dev/sda1
>    /dev/sda2
>    /dev/sda3

2. Mount the block device

```
# mount -t ext4 /dev/genramdisk /mnt/
# mount
/dev/root on / type ext4 (rw,relatime)
devtmpfs on /dev type devtmpfs (rw,relatime,size=790360k,nr_inodes=197590,mode=755)
proc on /proc type proc (rw,relatime)
devpts on /dev/pts type devpts (rw,relatime,gid=5,mode=620,ptmxmode=666)
tmpfs on /dev/shm type tmpfs (rw,relatime,mode=777)
tmpfs on /tmp type tmpfs (rw,relatime)
tmpfs on /run type tmpfs (rw,nosuid,nodev,relatime,mode=755)
sysfs on /sys type sysfs (rw,relatime)
/dev/ramdiskblockdev on /mnt type ext4 (rw,relatime)

# cd /mnt/
# echo "hello file system" > data.txt
# ls
data.txt    lost+found
```

3. Unmount and mount the block device to verify that the data is still there

```
# cd /
# umount /mnt

# mount -t ext4 /dev/genramdisk /mnt/
# cat /mnt/data.txt
hello file system
```

## Send a write and read requests to the block device

To send a write request to the device, we redirect the output of `echo` to
the device node.

> Note that before the `write` request is executed, a `read` request
> is performed first. This is because we might write in the middle of
> the sector without modifying the data before the offset.

```
# echo "abcd" > /dev/ramdiskblockdev
[  304.330699] ram_block_driver: ram_block_open: Open disk genramdisk
[  304.338545] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request
[  304.339256] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[  304.339355] ram_block_driver: pr_request: READ direction request
[  304.342292] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  0
[  304.343330] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 4096
[  304.349609] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 4096
[  304.351130] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[  304.351322] ram_block_driver: exec_request: Processing segments in bio
[  304.359611] ram_block_driver: exec_request: READ Segment 0
[  304.368838] ram_block_driver: exec_request:    sector:    0
[  304.369567] ram_block_driver: exec_request:    bv_offset: 0
[  304.374522] ram_block_driver: exec_request:    offset:    0
[  304.375052] ram_block_driver: exec_request:    len:       4096
[  304.390658] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request
[  304.393046] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[  304.393178] ram_block_driver: pr_request: WRITE direction request
[  304.394113] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  0
[  304.395296] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 4096
[  304.396268] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 4096
[  304.396773] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[  304.396852] ram_block_driver: exec_request: Processing segments in bio
[  304.397250] ram_block_driver: exec_request: WRITE Segment 0
[  304.397621] ram_block_driver: exec_request:    sector:    0
[  304.397945] ram_block_driver: exec_request:    bv_offset: 0
[  304.398251] ram_block_driver: exec_request:    offset:    0
[  304.398584] ram_block_driver: exec_request:    len:       4096
[  304.406501] ram_block_driver: ram_block_release: Release disk genramdisk
```

Read the data:

```
# dd if=/dev/ramdiskblockdev bs=1 count=10
abcd
�i
10+0 records in
10+0 records out
```

In the kernel logs we can see:

```
# [  473.704458] ram_block_driver: ram_block_open: Open disk genramdisk
[  473.706091] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request
[  473.706582] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[  473.706654] ram_block_driver: pr_request: READ direction request
[  473.707205] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  0
[  473.711209] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 16384
[  473.716386] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 4096
[  473.716962] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[  473.717062] ram_block_driver: exec_request: Processing segments in bio
[  473.717420] ram_block_driver: exec_request: READ Segment 0
[  473.717822] ram_block_driver: exec_request:    sector:    0
[  473.718175] ram_block_driver: exec_request:    bv_offset: 0
[  473.718501] ram_block_driver: exec_request:    offset:    0
[  473.718826] ram_block_driver: exec_request:    len:       4096
[  473.719166] ram_block_driver: exec_request: READ Segment 1
[  473.719590] ram_block_driver: exec_request:    sector:    8
[  473.720637] ram_block_driver: exec_request:    bv_offset: 0
[  473.721021] ram_block_driver: exec_request:    offset:    4096
[  473.721304] ram_block_driver: exec_request:    len:       4096
[  473.721626] ram_block_driver: exec_request: READ Segment 2
[  473.722045] ram_block_driver: exec_request:    sector:    16
[  473.722314] ram_block_driver: exec_request:    bv_offset: 0
[  473.722766] ram_block_driver: exec_request:    offset:    8192
[  473.723093] ram_block_driver: exec_request:    len:       4096
[  473.723537] ram_block_driver: exec_request: READ Segment 3
[  473.729914] ram_block_driver: exec_request:    sector:    24
[  473.730270] ram_block_driver: exec_request:    bv_offset: 0
[  473.730637] ram_block_driver: exec_request:    offset:    12288
[  473.730995] ram_block_driver: exec_request:    len:       4096
[  473.747105] ram_block_driver: ram_block_release: Release disk genramdisk
```

## Read and Write in a particular offset

First, we are going to create the `numbers.txt` file and print the 12th sector:

```
# for i in $(seq 1 25345); do echo $i >> numbers.txt ; done
# ls -la numbers.txt
-rw-r--r--    1 root     root        140964 Jan  2 05:11 numbers.txt

# dd if=./numbers.txt bs=512 count=1 skip=12
451
1452
1453
1454
1455
...
1550
1551
1552
155
1+0 records in
1+0 records out
```

Then, we will copy `131072` bytes to the block device, which it is the capacity
of the device. The capacity is calculated by multiplying the number of sectors
times the sector size.

```
#define NR_SECTORS				256
#define RAMDISK_SECTOR_SIZE		512
```

To copy the file, we run

```
# dd if=numbers.txt of=/dev/ramdiskblockdev
dd: error writing '/dev/ramdiskblockdev': No space left on device
257+0 records in
256+0 records out

# echo "256*512" | bc
131072
```

Only `256` sectors of `512` bytes were copied to `/dev/ramdiskblockdev`. To
print all the sectors, we run:

```
# dd if=/dev/ramdiskblockdev bs=512 count=257
1
2
..
..
23695
23696
23
256+0 records in
256+0 records out
```

Even if we tried to print `257`, there are only `256` sectors.

To print the content of the `12th` sector, we run:

```
# dd if=/dev/ramdiskblockdev bs=512 count=1 skip=12
451
1452
1453
1454
1455
...
1550
1551
1552
155
1+0 records in
1+0 records out
```

In the kernel logs, we can see that the driver starts reading from the 8th sector
4096 bytes (8 sectors).

```
# [ 1772.849081] ram_block_driver: ram_block_open: Open disk genramdisk
[ 1772.851629] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request
[ 1772.855040] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[ 1772.855164] ram_block_driver: pr_request: READ direction request
[ 1772.856645] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  8
[ 1772.857400] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 4096
[ 1772.857867] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 4096
[ 1772.858292] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[ 1772.858376] ram_block_driver: exec_request: Processing segments in bio
[ 1772.858692] ram_block_driver: exec_request: READ Segment 0
[ 1772.859026] ram_block_driver: exec_request:    sector:    8
[ 1772.859371] ram_block_driver: exec_request:    bv_offset: 0
[ 1772.864584] ram_block_driver: exec_request:    offset:    4096
[ 1772.865310] ram_block_driver: exec_request:    len:       4096
[ 1772.882519] ram_block_driver: ram_block_release: Release disk genramdisk
```

## Compare SHAs of the block device and file

To make sure that the content of the ramdisk matches the content of the 256 sectos
of the `numbers.txt`, we generate a new file `original.txt` that will only
contain 256 sectors of 512 bytes each. Then, we generate the `disk.txt` file
that will contain the data in `/dev/ramdiskblockdev`. Once we do that,
we use `md5sum` to compare SHAs.

```
# dd if=numbers.txt of=original.txt bs=512 count=256
256+0 records in
256+0 records out

# dd if=/dev/ramdiskblockdev of=disk.txt bs=512 count=256
256+0 records in
256+0 records out
# md5sum original.txt disk.txt
29a54dffd9978a29f112423b08ea0894  original.txt
29a54dffd9978a29f112423b08ea0894  disk.txt
```

## Insert text in a specific offset

To insert text in a specific offset in the disk, we can run the following
commands:

```
$ echo "Hello World" > message.txt
$ cat message.txt
Hello World

# To insert 11 bytes at the beginning of the 14th sector, we skip
# 14 sectors (14 * 512 = 7168 bytes) and insert 11 bytes.
$ dd if=message.txt of=/dev/ramdiskblockdev bs=1 seek=7168 count=11
11+0 records in
11+0 records out

# Verify the the message is in the ramdisk.
$ dd if=/dev/ramdiskblockdev | less
...
1654
1655
Hello World658
1659
1660
1661
1662
1663
...

```

## Remove the block device

To remove the block device, run:

```
# rmmod ram_block_driver

[ 3979.689309] ram_block_driver: ram_block_exit: Exiting ram block driver
[ 3979.689588] ram_block_driver: ram_block_exit: ram_blk_dev has [1] references(s)!
[ 3979.690332] ram_block_driver: release_block_device: Unregistering block I/O device
[ 3979.690965] ram_block_driver: release_block_device: Deleting gendisk
[ 3979.736829] ram_block_driver: release_block_device: Destroying queue
[ 3979.737864] ram_block_driver: release_block_device: Freeing tag_set
[ 3979.738802] ram_block_driver: release_block_device: Freeing ramdisk
```
