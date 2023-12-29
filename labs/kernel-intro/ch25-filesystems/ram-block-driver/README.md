
# Ram block device driver

This page describes how to build the block driver,
deploy it and test it.


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
[  518.833010] ram_block_driver: ram_block_init: Before register_blkdev()
[  518.833376] ram_block_driver: init_blk_mq_tag_set: Before blk_mq_alloc_tag_set()
[  518.839022] ram_block_driver: init_request_queue: Before blk_mq_init_queue()
[  518.843522] ram_block_driver: init_request_queue: Before blk_queue_physical_block_size()
[  518.844103] ram_block_driver: init_gendisk: Before blk_mq_alloc_disk()
[  518.859084] ram_block_driver: init_gendisk: Before set_capacity()
[  518.860625] ram_block_driver: setup_block_dev: Before add_disk()
[  518.893727] ram_block_driver: ram_block_open: Open disk genramdisk
[  518.904834] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request
[  518.905996] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[  518.906122] ram_block_driver: pr_request: READ direction request
[  518.912405] ram_block_driver: pr_request: __sector =  0
[  518.913266] ram_block_driver: pr_request: __data_len = 4096
[  518.921596] ram_block_driver: pr_request: blk_rq_cur_bytes = 4096
[  518.922138] ram_block_driver: pr_request: mq_rq_state = MQ_RQ_IDLE
[  518.926874] ram_block_driver: ram_block_release: Release disk genramdisk
[  518.928759] ram_block_driver: ram_block_init: Make sure to create the node:
```

## Verify that the block device and ramdisk are created

To verify that the block device and ramdisk were created,
run:

```
 $ cat /proc/devices | grep ramblockdev
 240 ramblockdev

 $ ls -la /dev/genramdisk
 brw-------    1 root     root      240,   0 Dec 28 04:58 /dev/genramdisk
```

To see the configurations of the disk:

```
# for f in `find /sys/block/genramdisk/ -type f`; do echo $f; cat $f; done;
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
       1        0        8       19        0        0        0        0        0       24       19        0        0        0        0        0        0
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
1024
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
0
/sys/block/genramdisk/hidden
0
/sys/block/genramdisk/removable
0
/sys/block/genramdisk/events
/sys/block/genramdisk/inflight
       0        0
```

