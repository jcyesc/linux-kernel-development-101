
# Read/Write using a `struct bio`


The `bio_block_driver.c` sends request to the block layer by creating
`struct bio` request and adding the kernel mapped page to the request.

The `bio_block_driver.c` depends on the
`ch25-filesystems/ram-block-driver/ram_block_driver.c` driver.

To run the example, follow the instructions.

1. Install the `block_driver.ko` and write a message to the ramdisk:

```
$ insmod ram_block_driver.ko
$ mknod /dev/ramdiskblockdev b 240 0
$ echo "Linux is a powerful and versatile OS" > /dev/ramdiskblockdev
$ dd if=/dev/ramdiskblockdev bs=1 count=50
Linux is a powerful and versatile OS
50+0 records in
50+0 records out
```

The kernel logs show:

```
[  374.475109] ram_block_driver: ram_block_init: Before register_blkdev()
[  374.475852] ram_block_driver: init_blk_mq_tag_set: Before blk_mq_alloc_tag_set()
[  374.486010] ram_block_driver: init_request_queue: Before blk_mq_init_queue()
[  374.500984] ram_block_driver: init_request_queue: Before blk_queue_physical_block_size()
[  374.502329] ram_block_driver: init_gendisk: Before blk_mq_alloc_disk()
[  374.519711] ram_block_driver: init_gendisk: Before set_capacity()
[  374.524468] ram_block_driver: setup_block_dev: Before add_disk()
[  374.578802] ram_block_driver: ram_block_open: Open disk genramdisk
[  374.593177] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request
[  374.594481] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[  374.594619] ram_block_driver: pr_request: READ direction request
[  374.595695] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  0
[  374.601768] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 4096
[  374.602223] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 4096
[  374.602557] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[  374.602678] ram_block_driver: exec_request: Processing segments in bio
[  374.603201] ram_block_driver: exec_request: READ Segment 0
[  374.603576] ram_block_driver: exec_request:    sector:    0
[  374.609467] ram_block_driver: exec_request:    bv_offset: 0
[  374.610382] ram_block_driver: exec_request:    offset:    0
[  374.610787] ram_block_driver: exec_request:    len:       4096
[  374.632730] ram_block_driver: ram_block_release: Release disk genramdisk
[  374.635215] ram_block_driver: ram_block_init: Make sure to create the node:
[  374.635323] ram_block_driver: ram_block_init:     mknod /dev/ramdiskblockdev b 240 0
[  390.718480] ram_block_driver: ram_block_open: Open disk genramdisk
[  390.727398] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request
[  390.729155] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[  390.729305] ram_block_driver: pr_request: READ direction request
[  390.730117] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  0
[  390.730924] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 4096
[  390.731540] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 4096
[  390.732714] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[  390.732830] ram_block_driver: exec_request: Processing segments in bio
[  390.733423] ram_block_driver: exec_request: READ Segment 0
[  390.733949] ram_block_driver: exec_request:    sector:    0
[  390.734460] ram_block_driver: exec_request:    bv_offset: 0
[  390.734890] ram_block_driver: exec_request:    offset:    0
[  390.735338] ram_block_driver: exec_request:    len:       4096
[  390.753131] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request
[  390.755555] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[  390.756385] ram_block_driver: pr_request: WRITE direction request
[  390.758173] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  0
[  390.765369] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 4096
[  390.774690] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 4096
[  390.775126] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[  390.775212] ram_block_driver: exec_request: Processing segments in bio
[  390.775633] ram_block_driver: exec_request: WRITE Segment 0
[  390.781376] ram_block_driver: exec_request:    sector:    0
[  390.787227] ram_block_driver: exec_request:    bv_offset: 0
[  390.790439] ram_block_driver: exec_request:    offset:    0
[  390.791680] ram_block_driver: exec_request:    len:       4096
[  390.803342] ram_block_driver: ram_block_release: Release disk genramdisk
[  399.437251] ram_block_driver: ram_block_open: Open disk genramdisk
[  399.451153] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request
[  399.451754] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[  399.453881] ram_block_driver: pr_request: READ direction request
[  399.454440] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  0
[  399.455110] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 16384
[  399.455544] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 4096
[  399.458463] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[  399.458588] ram_block_driver: exec_request: Processing segments in bio
[  399.459005] ram_block_driver: exec_request: READ Segment 0
[  399.459454] ram_block_driver: exec_request:    sector:    0
[  399.463088] ram_block_driver: exec_request:    bv_offset: 0
[  399.463511] ram_block_driver: exec_request:    offset:    0
[  399.466769] ram_block_driver: exec_request:    len:       4096
[  399.467238] ram_block_driver: exec_request: READ Segment 1
[  399.467529] ram_block_driver: exec_request:    sector:    8
[  399.469682] ram_block_driver: exec_request:    bv_offset: 0
[  399.470070] ram_block_driver: exec_request:    offset:    4096
[  399.470335] ram_block_driver: exec_request:    len:       4096
[  399.470826] ram_block_driver: exec_request: READ Segment 2
[  399.471193] ram_block_driver: exec_request:    sector:    16
[  399.471600] ram_block_driver: exec_request:    bv_offset: 0
[  399.482054] ram_block_driver: exec_request:    offset:    8192
[  399.483086] ram_block_driver: exec_request:    len:       4096
[  399.487147] ram_block_driver: exec_request: READ Segment 3
[  399.491517] ram_block_driver: exec_request:    sector:    24
[  399.495190] ram_block_driver: exec_request:    bv_offset: 0
[  399.505187] ram_block_driver: exec_request:    offset:    12288
[  399.505574] ram_block_driver: exec_request:    len:       4096
[  399.523833] ram_block_driver: ram_block_release: Release disk genramdisk
```

2. Install the `bio_block_driver.ko` that will send a write and read bio request.

```
$ insmod bio_block_driver.ko

$ dd if=/dev/ramdiskblockdev  bs=1 count=50 skip=0
This is the first message :)
50+0 records in
50+0 records out
$ dd if=/dev/ramdiskblockdev  bs=1 count=50 skip=512
Second segment
50+0 records in
50+0 records out
$ dd if=/dev/ramdiskblockdev  bs=1 count=50 skip=1024
The 3rd segment!@HPTX\dlp
50+0 records in
50+0 records out
$ dd if=/dev/ramdiskblockdev  bs=1 count=50 skip=1536
This is the last segment or the fourth one :)
50+0 records in
50+0 records out
```

The kernel logs are:

```
[  638.198709] bio_block_driver: bio_block_init: Starting bio block driver
[  638.199282] ram_block_driver: ram_block_open: Open disk genramdisk
[  638.202854] bio_block_driver: submit_write_request: segment = 0, vec_entry_len = 512
[  638.203004] bio_block_driver: submit_write_request: segment = 1, vec_entry_len = 512
[  638.203520] bio_block_driver: submit_write_request: segment = 2, vec_entry_len = 512
[  638.205906] bio_block_driver: submit_write_request: segment = 3, vec_entry_len = 512
[  638.206673] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request
[  638.207461] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[  638.207531] ram_block_driver: pr_request: WRITE direction request
[  638.210339] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  0
[  638.211207] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 2048
[  638.211686] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 512
[  638.212720] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[  638.212829] ram_block_driver: exec_request: Processing segments in bio
[  638.213257] ram_block_driver: exec_request: WRITE Segment 0
[  638.213662] ram_block_driver: exec_request:    sector:    0
[  638.214022] ram_block_driver: exec_request:    bv_offset: 0
[  638.214341] ram_block_driver: exec_request:    offset:    0
[  638.214752] ram_block_driver: exec_request:    len:       512
[  638.215040] ram_block_driver: exec_request: WRITE Segment 1
[  638.215600] ram_block_driver: exec_request:    sector:    1
[  638.221387] ram_block_driver: exec_request:    bv_offset: 0
[  638.221884] ram_block_driver: exec_request:    offset:    512
[  638.222244] ram_block_driver: exec_request:    len:       512
[  638.222543] ram_block_driver: exec_request: WRITE Segment 2
[  638.222830] ram_block_driver: exec_request:    sector:    2
[  638.223102] ram_block_driver: exec_request:    bv_offset: 0
[  638.223379] ram_block_driver: exec_request:    offset:    1024
[  638.223653] ram_block_driver: exec_request:    len:       512
[  638.230731] ram_block_driver: exec_request: WRITE Segment 3
[  638.232761] ram_block_driver: exec_request:    sector:    3
[  638.233582] ram_block_driver: exec_request:    bv_offset: 0
[  638.234277] ram_block_driver: exec_request:    offset:    1536
[  638.234857] ram_block_driver: exec_request:    len:       512
[  638.242643] bio_block_driver: submit_read_request: segment = 1, vec_entry_len = 2048
[  638.245968] ram_block_driver: blk_mq_ops_ram_queue_rq: Queuing request
[  638.246961] ram_block_driver: blk_mq_ops_ram_queue_rq: This is the 'last' request in the queue
[  638.247045] ram_block_driver: pr_request: READ direction request
[  638.247554] ram_block_driver: pr_request: 	blk_rq_pos = __sector =  0
[  638.252962] ram_block_driver: pr_request: 	blk_rq_bytes = __data_len = 2048
[  638.253585] ram_block_driver: pr_request: 	blk_rq_cur_bytes = 2048
[  638.254206] ram_block_driver: pr_request: 	mq_rq_state = MQ_RQ_IDLE
[  638.254282] ram_block_driver: exec_request: Processing segments in bio
[  638.254761] ram_block_driver: exec_request: READ Segment 0
[  638.255108] ram_block_driver: exec_request:    sector:    0
[  638.255379] ram_block_driver: exec_request:    bv_offset: 0
[  638.256624] ram_block_driver: exec_request:    offset:    0
[  638.257187] ram_block_driver: exec_request:    len:       2048
[  638.257807] bio_block_driver: submit_read_request: T h i s
[  638.258492] bio_block_driver: submit_read_request: S e c o n
[  638.259254] bio_block_driver: submit_read_request: T h e   3
[  638.267512] bio_block_driver: submit_read_request: T h i s
```

