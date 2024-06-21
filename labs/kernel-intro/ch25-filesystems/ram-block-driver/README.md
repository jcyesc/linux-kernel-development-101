
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
# Create a block node.
qemu $ mknod /dev/rdbd b 240 0
qemu $ insmod ram_block_driver.ko
[ 1537.891323] ram_block_driver: loading out-of-tree module taints kernel.
[ 1537.896681] ram_block_driver: ram_block_init: Before register_blkdev()
[ 1537.904347] ram_block_driver: init_blk_mq_tag_set: Before blk_mq_alloc_tag_set()
[ 1537.904818] ram_block_driver: init_request_queue: Before blk_mq_init_queue()
[ 1537.905192] ram_block_driver: init_request_queue: Before blk_queue_physical_block_size()
[ 1537.905490] blk_queue_max_segment_size: set to minimum 4096
[ 1537.906012] ram_block_driver: init_gendisk: Before blk_mq_alloc_disk()
[ 1537.906471] ram_block_driver: init_gendisk: Before set_capacity()
[ 1537.906792] ram_block_driver: setup_block_dev: Before add_disk()
[ 1537.910232] ram_block_driver: ram_block_init: Make sure to create the node:
[ 1537.910516] ram_block_driver: ram_block_init:     mknod /dev/rdbd b 240 0
```

## Verify that the block device, partition and ramdisk are created

To verify that the partition was created, run:

```
 $ cat /proc/partitions
major minor  #blocks  name

 254        0     614400 vda
  31        0     131072 mtdblock0
 240        0      65536 rdbd1
```

To verify that the block device and gendisk were created, run:

```
 # Verify that block device exists.
 $ cat /proc/devices | grep rdbd
240 rdbd

 # Verify that block device and gendisk exists.
 $ ls -la /dev/rdbd*
brw-r--r--    1 root     root      240,   0 May 13 09:46 /dev/rdbd
brw-------    1 root     root      240,   1 May 13 09:47 /dev/rdbd1
```

To see the configurations of the disk:

```
 $ for f in `find /sys/block/rdbd1/ -type f`; do echo $f; cat $f; done;
/sys/block/rdbd1/uevent
MAJOR=240
MINOR=1
DEVNAME=rdbd1
DEVTYPE=disk
DISKSEQ=11
/sys/block/rdbd1/ext_range
1
/sys/block/rdbd1/range
1
/sys/block/rdbd1/alignment_offset
0
/sys/block/rdbd1/diskseq
11
/sys/block/rdbd1/power/runtime_active_time
0
/sys/block/rdbd1/power/runtime_status
unsupported
/sys/block/rdbd1/power/autosuspend_delay_ms
cat: read error: Input/output error
/sys/block/rdbd1/power/runtime_suspended_time
0
/sys/block/rdbd1/power/control
auto
/sys/block/rdbd1/dev
240:1
/sys/block/rdbd1/ro
0
/sys/block/rdbd1/mq/0/cpu_list
0
/sys/block/rdbd1/mq/0/nr_reserved_tags
0
/sys/block/rdbd1/mq/0/nr_tags
16
/sys/block/rdbd1/stat
       0        0        0        0        0        0        0        0        0        0        0        0        0        0        0        0        0
/sys/block/rdbd1/events_poll_msecs
-1
/sys/block/rdbd1/events_async
/sys/block/rdbd1/queue/io_poll_delay
-1
/sys/block/rdbd1/queue/max_integrity_segments
0
/sys/block/rdbd1/queue/zoned
none
/sys/block/rdbd1/queue/scheduler
none [mq-deadline] kyber bfq
/sys/block/rdbd1/queue/io_poll
0
/sys/block/rdbd1/queue/discard_zeroes_data
0
/sys/block/rdbd1/queue/minimum_io_size
512
/sys/block/rdbd1/queue/nr_zones
0
/sys/block/rdbd1/queue/write_same_max_bytes
0
/sys/block/rdbd1/queue/max_segments
128
/sys/block/rdbd1/queue/dax
0
/sys/block/rdbd1/queue/dma_alignment
511
/sys/block/rdbd1/queue/physical_block_size
512
/sys/block/rdbd1/queue/logical_block_size
512
/sys/block/rdbd1/queue/virt_boundary_mask
0
/sys/block/rdbd1/queue/zone_append_max_bytes
0
/sys/block/rdbd1/queue/nr_requests
32
/sys/block/rdbd1/queue/write_cache
write through
/sys/block/rdbd1/queue/stable_writes
0
/sys/block/rdbd1/queue/max_segment_size
4096
/sys/block/rdbd1/queue/rotational
0
/sys/block/rdbd1/queue/discard_max_bytes
0
/sys/block/rdbd1/queue/add_random
0
/sys/block/rdbd1/queue/discard_max_hw_bytes
0
/sys/block/rdbd1/queue/optimal_io_size
0
/sys/block/rdbd1/queue/chunk_sectors
0
/sys/block/rdbd1/queue/iosched/front_merges
1
/sys/block/rdbd1/queue/iosched/read_expire
500
/sys/block/rdbd1/queue/iosched/prio_aging_expire
10000
/sys/block/rdbd1/queue/iosched/fifo_batch
16
/sys/block/rdbd1/queue/iosched/write_expire
5000
/sys/block/rdbd1/queue/iosched/writes_starved
2
/sys/block/rdbd1/queue/iosched/async_depth
6
/sys/block/rdbd1/queue/read_ahead_kb
128
/sys/block/rdbd1/queue/max_discard_segments
1
/sys/block/rdbd1/queue/write_zeroes_max_bytes
0
/sys/block/rdbd1/queue/nomerges
0
/sys/block/rdbd1/queue/zone_write_granularity
0
/sys/block/rdbd1/queue/fua
0
/sys/block/rdbd1/queue/discard_granularity
0
/sys/block/rdbd1/queue/rq_affinity
1
/sys/block/rdbd1/queue/max_sectors_kb
127
/sys/block/rdbd1/queue/hw_sector_size
512
/sys/block/rdbd1/queue/max_hw_sectors_kb
127
/sys/block/rdbd1/queue/iostats
1
/sys/block/rdbd1/size
131072
/sys/block/rdbd1/integrity/write_generate
0
/sys/block/rdbd1/integrity/format
none
/sys/block/rdbd1/integrity/read_verify
0
/sys/block/rdbd1/integrity/tag_size
0
/sys/block/rdbd1/integrity/protection_interval_bytes
0
/sys/block/rdbd1/integrity/device_is_integrity_capable
0
/sys/block/rdbd1/discard_alignment
0
/sys/block/rdbd1/capability
0
/sys/block/rdbd1/hidden
0
/sys/block/rdbd1/removable
0
/sys/block/rdbd1/events
/sys/block/rdbd1/inflight
       0        0
```

## Format the block device and mount a file system

1. Format the first generic disk with ext4

```
 $ mke2fs /dev/rdbd1
mke2fs 1.47.0 (5-Feb-2023)
Creating filesystem with 65536 1k blocks and 16384 inodes
Filesystem UUID: c9f9fe75-73cc-4d3a-b044-1779ba689b3e
Superblock backups stored on blocks:
	8193, 24577, 40961, 57345

Allocating group tables: done
Writing inode tables: done
Writing superblocks and filesystem accounting information: done

 $ file -s /dev/rdbd1
/dev/rdbd1: Linux rev 1.0 ext2 filesystem data, UUID=c9f9fe75-73cc-4d3a-b044-1779ba689b3e (large files)
```

Because the device node `/dev/rdbd` was created with different minor version (`0`)
than `/dev/rdbd1`, we are unable to read/write on `/dev/rdbd`.

```
 $ echo "hello" > /dev/rdbd
-sh: can't create /dev/rdbd: No such device or address

 $ mke2fs /dev/rdbd
mke2fs 1.47.0 (5-Feb-2023)
mke2fs: No such device or address while trying to determine filesystem size

 $ file -s /dev/rdbd
/dev/rdbd: writable, no read permission
```

2. Mount the block device

```
 $ mount -t ext4 /dev/rdbd1 /mnt/
 $ mount
/dev/root on / type ext4 (rw,relatime)
devtmpfs on /dev type devtmpfs (rw,relatime,size=957816k,nr_inodes=239454,mode=755)
proc on /proc type proc (rw,relatime)
devpts on /dev/pts type devpts (rw,relatime,gid=5,mode=620,ptmxmode=666)
tmpfs on /dev/shm type tmpfs (rw,relatime,mode=777)
tmpfs on /tmp type tmpfs (rw,relatime)
tmpfs on /run type tmpfs (rw,nosuid,nodev,relatime,mode=755)
sysfs on /sys type sysfs (rw,relatime)
/dev/rdbd1 on /mnt type ext4 (rw,relatime)

 $ ls /sys/fs/ext4/
features  rdbd1     vda

 $ ls /sys/fs/ext4/rdbd1/
delayed_allocation_blocks      inode_goal                     max_writeback_mb_bump          msg_count
err_ratelimit_burst            inode_readahead_blks           mb_best_avail_max_trim_order   msg_ratelimit_burst
err_ratelimit_interval_ms      journal_task                   mb_group_prealloc              msg_ratelimit_interval_ms
errors_count                   last_error_block               mb_max_linear_groups           reserved_clusters
extent_max_zeroout_kb          last_error_errcode             mb_max_to_scan                 session_write_kbytes
first_error_block              last_error_func                mb_min_to_scan                 sra_exceeded_retry_limit
first_error_errcode            last_error_ino                 mb_order2_req                  trigger_fs_error
first_error_func               last_error_line                mb_prefetch                    warning_count
first_error_ino                last_error_time                mb_prefetch_limit              warning_ratelimit_burst
first_error_line               last_trim_minblks              mb_stats                       warning_ratelimit_interval_ms
first_error_time               lifetime_write_kbytes          mb_stream_req

 $ cat /sys/fs/ext4/rdbd1/first_error_errcode
0

 $ cd /mnt/
 $ echo "hello file system" > data.txt
 $ ls
data.txt    lost+found
```

3. Unmount and mount the block device to verify that the data is still there

```
 $ cd /
 $ umount /mnt

 $ mount -t ext4 /dev/rdbd1 /mnt/
 $ cat /mnt/data.txt
hello file system
```

## Send a write and read requests to the block device

To send a write request to the device, we redirect the output of `echo` to
the device node.

> Note that before the `write` request is executed, a `read` request
> is performed first. This is because we might write in the middle of
> the sector without modifying the data before the offset.

Whe we write directly to the disk `/dev/rdbd1`, we might corrupt the filesystem
in there, if there is one.

```
 $ echo "abcd" > /dev/rdbd1
[ 3393.577327] ram_block_driver: exec_request:    len:       1024
[ 3545.204951] ram_block_driver: ram_block_open: Open disk rdbd1
[ 3545.206956] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request for disk 'rdbd1'
[ 3545.209004] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[ 3545.209054] ram_block_driver: pr_request: READ direction request
[ 3545.209529] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  0
[ 3545.210063] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 1024
[ 3545.210421] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 1024
[ 3545.210704] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[ 3545.210737] ram_block_driver: exec_request: Processing segments in bio
[ 3545.211107] ram_block_driver: exec_request: READ Segment 0
[ 3545.211450] ram_block_driver: exec_request:    sector:    0
[ 3545.213213] ram_block_driver: exec_request:    bv_offset: 0
[ 3545.213498] ram_block_driver: exec_request:    offset:    0
[ 3545.213779] ram_block_driver: exec_request:    len:       1024
[ 3545.214363] ram_block_driver: ram_block_release: Release disk rdbd1
[ 3575.830615] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request for disk 'rdbd1'
[ 3575.831947] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[ 3575.831986] ram_block_driver: pr_request: WRITE direction request
[ 3575.832568] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  0
[ 3575.833411] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 1024
[ 3575.834127] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 1024
[ 3575.834556] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[ 3575.834588] ram_block_driver: exec_request: Processing segments in bio
[ 3575.834929] ram_block_driver: exec_request: WRITE Segment 0
[ 3575.836703] ram_block_driver: exec_request:    sector:    0
[ 3575.837104] ram_block_driver: exec_request:    bv_offset: 0
[ 3575.837549] ram_block_driver: exec_request:    offset:    0
```

Read the data:

```
 $ dd if=/dev/rdbd1 bs=1 count=10
abcd
�i
10+0 records in
10+0 records out
```

In the kernel logs we can see:

```
[ 3702.065518] ram_block_driver: ram_block_open: Open disk rdbd1
[ 3702.066438] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request for disk 'rdbd1'
[ 3702.073915] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[ 3702.073966] ram_block_driver: pr_request: READ direction request
[ 3702.074294] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  0
[ 3702.074699] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 16384
[ 3702.074872] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 4096
[ 3702.075193] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[ 3702.075224] ram_block_driver: exec_request: Processing segments in bio
[ 3702.078207] ram_block_driver: exec_request: READ Segment 0
[ 3702.079024] ram_block_driver: exec_request:    sector:    0
[ 3702.081654] ram_block_driver: exec_request:    bv_offset: 0
[ 3702.082398] ram_block_driver: exec_request:    offset:    0
[ 3702.082736] ram_block_driver: exec_request:    len:       4096
[ 3702.083394] ram_block_driver: exec_request: READ Segment 1
[ 3702.085904] ram_block_driver: exec_request:    sector:    8
[ 3702.088168] ram_block_driver: exec_request:    bv_offset: 0
[ 3702.088686] ram_block_driver: exec_request:    offset:    4096
[ 3702.089171] ram_block_driver: exec_request:    len:       4096
[ 3702.089735] ram_block_driver: exec_request: READ Segment 2
[ 3702.090242] ram_block_driver: exec_request:    sector:    16
[ 3702.090736] ram_block_driver: exec_request:    bv_offset: 0
[ 3702.091234] ram_block_driver: exec_request:    offset:    8192
[ 3702.093818] ram_block_driver: exec_request:    len:       4096
[ 3702.094148] ram_block_driver: exec_request: READ Segment 3
[ 3702.094433] ram_block_driver: exec_request:    sector:    24
[ 3702.094616] ram_block_driver: exec_request:    bv_offset: 0
[ 3702.094916] ram_block_driver: exec_request:    offset:    12288
[ 3702.095172] ram_block_driver: exec_request:    len:       4096
[ 3702.098256] ram_block_driver: ram_block_release: Release disk rdbd1
```

## Read and Write in a particular offset

First, we are going to create the `numbers.txt` file and print the 12th sector:

```
 $ for i in $(seq 1 25345); do echo $i >> numbers.txt ; done
 $ ls -la numbers.txt
-rw-r--r--    1 root     root        140964 Jan  2 05:11 numbers.txt

 $ dd if=./numbers.txt bs=512 count=1 skip=12
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

Then, we will copy `131072` bytes to the generic disk. The disk capacity is
calculated by multiplying the number of sectors times the sector size.

```
#define NR_SECTORS				131072
#define RAMDISK_SECTOR_SIZE		512
```

To copy the file, we run

```
 $ dd if=numbers.txt of=/dev/rdbd1
275+1 records in
275+1 records out
```

Note: If there were not enough capacity in the generic disk, we would get:

```
dd: error writing '/dev/rdbd1': No space left on device
257+0 records in
256+0 records out
```

To print `256` sectors of `512` bytes, we run:

```
 $ dd if=/dev/rdbd1 bs=512 count=257
1
2
..
..
23775
23776
23777
23778
23779
23780
23781
2378
257+0 records in
257+0 records out
```

To print the content of the `12th` sector, we run:

```
 $ dd if=/dev/rdbd1 bs=512 count=1 skip=12
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
[ 4677.454711] ram_block_driver: ram_block_open: Open disk rdbd1
[ 4677.456886] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request for disk 'rdbd1'
[ 4677.457285] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[ 4677.457305] ram_block_driver: pr_request: READ direction request
[ 4677.457682] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  8
[ 4677.458233] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 4096
[ 4677.458515] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 4096
[ 4677.458772] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[ 4677.458800] ram_block_driver: exec_request: Processing segments in bio
[ 4677.459110] ram_block_driver: exec_request: READ Segment 0
[ 4677.461280] ram_block_driver: exec_request:    sector:    8
[ 4677.461704] ram_block_driver: exec_request:    bv_offset: 0
[ 4677.461962] ram_block_driver: exec_request:    offset:    4096
[ 4677.462218] ram_block_driver: exec_request:    len:       4096
[ 4677.476894] ram_block_driver: ram_block_release: Release disk rdbd1
```

## Compare SHAs of the block device and file

To make sure that the content of the ramdisk matches the content of the 256 sectos
of the `numbers.txt`, we generate a new file `original.txt` that will only
contain 256 sectors of 512 bytes each. Then, we generate the `disk.txt` file
that will contain the data in `/dev/rdbd1`. Once we do that,
we use `md5sum` to compare SHAs.

```
 $ dd if=numbers.txt of=original.txt bs=512 count=256
256+0 records in
256+0 records out

 $ dd if=/dev/rdbd1 of=disk.txt bs=512 count=256
256+0 records in
256+0 records out
 $ md5sum original.txt disk.txt
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
 $ dd if=message.txt of=/dev/rdbd1 bs=1 seek=7168 count=11
11+0 records in
11+0 records out

# Verify the the message is in the ramdisk.
$ dd if=/dev/rdbd1 | less
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
 $ rmmod ram_block_driver.ko

[ 5250.591042] ram_block_driver: ram_block_release: Release disk rdbd1
[ 5268.595362] ram_block_driver: ram_block_exit: Exiting ram block driver
[ 5268.595651] ram_block_driver: ram_block_exit: ram_blk_dev has [1] references(s)!
[ 5268.596077] ram_block_driver: release_block_device: Unregistering block I/O device
[ 5268.596565] ram_block_driver: release_block_device: Deleting gendisk
[ 5268.623248] ram_block_driver: release_block_device: Destroying queue
[ 5268.623248] ram_block_driver: release_block_device: Freeing ramdisk
```
