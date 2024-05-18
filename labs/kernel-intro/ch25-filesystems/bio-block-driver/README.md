
# Read/Write using a `struct bio`


The `bio_block_driver.c` sends request to the block layer by creating
`struct bio` request and adding the kernel mapped page to the request.

The `bio_block_driver.c` depends on the
`ch25-filesystems/ram-block-driver/ram_block_driver.c` driver.

To run the example, follow the instructions.

1. Install the `block_driver.ko` and write a message to the ramdisk:

```
$ insmod ram_block_driver.ko
$ mknod /dev/rdbd b 240 0

# Write something in the generic disk #1.
$ echo "Linux is a powerful and versatile OS" > /dev/rdbd1
$ dd if=/dev/rdbd1 bs=1 count=50
Linux is a powerful and versatile OS
50+0 records in
50+0 records out
```

The kernel logs show:

```
[  251.332013] ram_block_driver: loading out-of-tree module taints kernel.
[  251.347877] ram_block_driver: ram_block_init: Before register_blkdev()
[  251.354721] ram_block_driver: init_blk_mq_tag_set: Before blk_mq_alloc_tag_set()
[  251.356698] ram_block_driver: init_request_queue: Before blk_mq_init_queue()
[  251.359856] ram_block_driver: init_request_queue: Before blk_queue_physical_block_size()
[  251.360288] blk_queue_max_segment_size: set to minimum 4096
[  251.360800] ram_block_driver: init_gendisk: Before blk_mq_alloc_disk()
[  251.364194] ram_block_driver: init_gendisk: Before set_capacity()
[  251.364537] ram_block_driver: setup_block_dev: Before add_disk()
[  251.374822] ram_block_driver: ram_block_init: Make sure to create the node:
[  251.375245] ram_block_driver: ram_block_init:     mknod /dev/rdbd b 240 0
[  410.573246] ram_block_driver: ram_block_open: Open disk rdbd1
[  410.580348] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request for disk 'rdbd1'
[  410.581012] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[  410.581052] ram_block_driver: pr_request: READ direction request
[  410.581992] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  0
[  410.582791] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 4096
[  410.583370] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 4096
[  410.583742] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[  410.583800] ram_block_driver: exec_request: Processing segments in bio
[  410.584321] ram_block_driver: exec_request: READ Segment 0
[  410.584826] ram_block_driver: exec_request:    sector:    0
[  410.586509] ram_block_driver: exec_request:    bv_offset: 0
[  410.586820] ram_block_driver: exec_request:    offset:    0
[  410.587092] ram_block_driver: exec_request:    len:       4096
[  410.590840] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request for disk 'rdbd1'
[  410.591770] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[  410.591806] ram_block_driver: pr_request: WRITE direction request
[  410.592222] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  0
[  410.592961] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 4096
[  410.594531] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 4096
[  410.594983] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[  410.595005] ram_block_driver: exec_request: Processing segments in bio
[  410.595419] ram_block_driver: exec_request: WRITE Segment 0
[  410.595950] ram_block_driver: exec_request:    sector:    0
[  410.596306] ram_block_driver: exec_request:    bv_offset: 0
[  410.596571] ram_block_driver: exec_request:    offset:    0
[  410.596930] ram_block_driver: exec_request:    len:       4096
[  410.602099] ram_block_driver: ram_block_release: Release disk rdbd1
[  418.844236] ram_block_driver: ram_block_open: Open disk rdbd1
[  418.847274] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request for disk 'rdbd1'
[  418.847710] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[  418.847729] ram_block_driver: pr_request: READ direction request
[  418.848062] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  0
[  418.848627] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 16384
[  418.848905] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 4096
[  418.849141] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[  418.851311] ram_block_driver: exec_request: Processing segments in bio
[  418.851742] ram_block_driver: exec_request: READ Segment 0
[  418.852039] ram_block_driver: exec_request:    sector:    0
[  418.852269] ram_block_driver: exec_request:    bv_offset: 0
[  418.852532] ram_block_driver: exec_request:    offset:    0
[  418.852763] ram_block_driver: exec_request:    len:       4096
[  418.853012] ram_block_driver: exec_request: READ Segment 1
[  418.853443] ram_block_driver: exec_request:    sector:    8
[  418.853765] ram_block_driver: exec_request:    bv_offset: 0
[  418.853984] ram_block_driver: exec_request:    offset:    4096
[  418.854233] ram_block_driver: exec_request:    len:       4096
[  418.854510] ram_block_driver: exec_request: READ Segment 2
[  418.854743] ram_block_driver: exec_request:    sector:    16
[  418.855013] ram_block_driver: exec_request:    bv_offset: 0
[  418.855227] ram_block_driver: exec_request:    offset:    8192
[  418.855482] ram_block_driver: exec_request:    len:       4096
[  418.855721] ram_block_driver: exec_request: READ Segment 3
[  418.855978] ram_block_driver: exec_request:    sector:    24
[  418.856200] ram_block_driver: exec_request:    bv_offset: 0
[  418.856442] ram_block_driver: exec_request:    offset:    12288
[  418.856686] ram_block_driver: exec_request:    len:       4096
[  418.861301] ram_block_driver: ram_block_release: Release disk rdbd1
```

2. Install the `bio_block_driver.ko` that will send a write and read bio request.

```
$ insmod bio_block_driver.ko
```

The kernel logs are:

```
[ 1214.912002] bio_block_driver: bio_block_init: Starting bio block driver
[ 1214.912757] ram_block_driver: ram_block_open: Open disk rdbd1
[ 1214.913547] bio_block_driver: submit_write_request: segment = 0, vec_entry_len = 512
[ 1214.913587] bio_block_driver: submit_write_request: segment = 1, vec_entry_len = 512
[ 1214.915460] bio_block_driver: submit_write_request: segment = 2, vec_entry_len = 512
[ 1214.915836] bio_block_driver: submit_write_request: segment = 3, vec_entry_len = 512
[ 1214.916228] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request for disk 'rdbd1'
[ 1214.916768] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[ 1214.916783] ram_block_driver: pr_request: WRITE direction request
[ 1214.917519] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  0
[ 1214.917897] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 2048
[ 1214.918346] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 512
[ 1214.918791] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[ 1214.918815] ram_block_driver: exec_request: Processing segments in bio
[ 1214.919468] ram_block_driver: exec_request: WRITE Segment 0
[ 1214.920547] ram_block_driver: exec_request:    sector:    0
[ 1214.920968] ram_block_driver: exec_request:    bv_offset: 0
[ 1214.925282] ram_block_driver: exec_request:    offset:    0
[ 1214.925633] ram_block_driver: exec_request:    len:       512
[ 1214.927252] ram_block_driver: exec_request: WRITE Segment 1
[ 1214.927733] ram_block_driver: exec_request:    sector:    1
[ 1214.928014] ram_block_driver: exec_request:    bv_offset: 0
[ 1214.928281] ram_block_driver: exec_request:    offset:    512
[ 1214.928692] ram_block_driver: exec_request:    len:       512
[ 1214.929008] ram_block_driver: exec_request: WRITE Segment 2
[ 1214.933710] ram_block_driver: exec_request:    sector:    2
[ 1214.934271] ram_block_driver: exec_request:    bv_offset: 0
[ 1214.935514] ram_block_driver: exec_request:    offset:    1024
[ 1214.936482] ram_block_driver: exec_request:    len:       512
[ 1214.938421] ram_block_driver: exec_request: WRITE Segment 3
[ 1214.939066] ram_block_driver: exec_request:    sector:    3
[ 1214.939458] ram_block_driver: exec_request:    bv_offset: 0
[ 1214.939784] ram_block_driver: exec_request:    offset:    1536
[ 1214.940098] ram_block_driver: exec_request:    len:       512
[ 1214.942168] bio_block_driver: submit_read_request: segment = 1, vec_entry_len = 2048
[ 1214.942619] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request for disk 'rdbd1'
[ 1214.943229] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[ 1214.943244] ram_block_driver: pr_request: READ direction request
[ 1214.943545] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  0
[ 1214.943977] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 2048
[ 1214.944211] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 2048
[ 1214.944475] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[ 1214.944493] ram_block_driver: exec_request: Processing segments in bio
[ 1214.944736] ram_block_driver: exec_request: READ Segment 0
[ 1214.944915] ram_block_driver: exec_request:    sector:    0
[ 1214.945112] ram_block_driver: exec_request:    bv_offset: 0
[ 1214.947025] ram_block_driver: exec_request:    offset:    0
[ 1214.947297] ram_block_driver: exec_request:    len:       2048
[ 1214.947660] bio_block_driver: submit_read_request: T h i s
[ 1214.948028] bio_block_driver: submit_read_request: S e c o n
[ 1214.948296] bio_block_driver: submit_read_request: T h e   3
```

3. Verify that the content is in `/dev/rdbd1`.

```
$ dd if=/dev/rdbd1 bs=1 count=50 skip=0
This is the first message :)
50+0 records in
50+0 records out

$ dd if=/dev/rdbd1  bs=1 count=50 skip=512
Second segment
50+0 records in
50+0 records out

$ dd if=/dev/rdbd1  bs=1 count=50 skip=1024
The 3rd segment!@HPTX\dlp
50+0 records in
50+0 records out

$ dd if=/dev/rdbd1  bs=1 count=50 skip=1536
This is the last segment or the fourth one :)
50+0 records in
50+0 records out
```

The kernel logs are:

```
[ 1629.485893] ram_block_driver: ram_block_open: Open disk rdbd1
[ 1629.487038] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request for disk 'rdbd1'
[ 1629.487406] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[ 1629.487423] ram_block_driver: pr_request: READ direction request
[ 1629.487813] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  0
[ 1629.488365] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 16384
[ 1629.488771] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 4096
[ 1629.489098] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[ 1629.489450] ram_block_driver: exec_request: Processing segments in bio
[ 1629.489910] ram_block_driver: exec_request: READ Segment 0
[ 1629.490291] ram_block_driver: exec_request:    sector:    0
[ 1629.490511] ram_block_driver: exec_request:    bv_offset: 0
[ 1629.490894] ram_block_driver: exec_request:    offset:    0
[ 1629.491169] ram_block_driver: exec_request:    len:       4096
[ 1629.491511] ram_block_driver: exec_request: READ Segment 1
[ 1629.491851] ram_block_driver: exec_request:    sector:    8
[ 1629.492165] ram_block_driver: exec_request:    bv_offset: 0
[ 1629.492521] ram_block_driver: exec_request:    offset:    4096
[ 1629.492839] ram_block_driver: exec_request:    len:       4096
[ 1629.493150] ram_block_driver: exec_request: READ Segment 2
[ 1629.493486] ram_block_driver: exec_request:    sector:    16
[ 1629.496539] ram_block_driver: exec_request:    bv_offset: 0
[ 1629.496832] ram_block_driver: exec_request:    offset:    8192
[ 1629.497113] ram_block_driver: exec_request:    len:       4096
[ 1629.497451] ram_block_driver: exec_request: READ Segment 3
[ 1629.497771] ram_block_driver: exec_request:    sector:    24
[ 1629.498087] ram_block_driver: exec_request:    bv_offset: 0
[ 1629.498447] ram_block_driver: exec_request:    offset:    12288
[ 1629.498762] ram_block_driver: exec_request:    len:       4096
[ 1629.514481] ram_block_driver: ram_block_release: Release disk rdbd1
 
[ 1687.791274] ram_block_driver: ram_block_open: Open disk rdbd1
[ 1687.801776] ram_block_driver: ram_block_release: Release disk rdbd1

[ 1711.321443] ram_block_driver: ram_block_open: Open disk rdbd1
[ 1711.326223] ram_block_driver: ram_block_release: Release disk rdbd1

[ 1771.028500] ram_block_driver: ram_block_open: Open disk rdbd1
[ 1771.050375] ram_block_driver: ram_block_release: Release disk rdbd1
```

Things to highlight:

- VFS sent a `struct bio` request with 4 read segments. The length of every
segment is `4096`, so this means that it read `16384 bytes` at once. This data
will be in the `page cache`.
- Subsequent read requests to contiguous data might not need disk access, as
the logs shown in `[ 1687.791274]`, `[ 1711.321443]` and `[ 1771.028500]`.

