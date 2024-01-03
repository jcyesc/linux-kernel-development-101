
# Chapter 25 - Filesystems


## Block layer

The block layer is in charge of storing the data blocks in the block device.
The block  driver is used to talk with the block device directly. The block
driver uses hardware queues to send the request to the block device.

The main data structures used in the block layer are:

- [struct block_device_operations](https://elixir.bootlin.com/linux/v6.6.2/source/include/linux/blkdev.h#L1375)
- [struct blk_mq_tag_set](https://elixir.bootlin.com/linux/v6.6.2/source/include/linux/blk-mq.h#L498)
- [struct request_queue](https://elixir.bootlin.com/linux/v6.6.2/source/include/linux/blkdev.h#L378)
- [struct blk_mq_ops](https://elixir.bootlin.com/linux/v6.6.2/source/include/linux/blk-mq.h#L537)
- [struct gendisk](https://elixir.bootlin.com/linux/v6.6.2/source/include/linux/blkdev.h#L128)
- [struct request](https://elixir.bootlin.com/linux/v6.6.2/source/include/linux/blk-mq.h#L80)
- [struct bio](https://elixir.bootlin.com/linux/v6.6.2/source/include/linux/blk_types.h#L264)
- [struct bio_vec](https://elixir.bootlin.com/linux/v6.6.2/source/include/linux/bvec.h#L31)
- [struct req_iterator](https://elixir.bootlin.com/linux/v6.6.2/source/include/linux/blk-mq.h#L1002)

### High level steps to process a read/write request

1. The userspace make a request to read a file.
2. The VFS processes the userspace request and hand it over to the filesystem.
3. The filesystem processes the VFS request and generates the `request` with
the `bio` structs populated.
4. The filesystem passes the `request` to the block driver.
5. The block driver processes the `request` and extracts the `bio` structs. Each
`bio` struct contains several segments represented by `bio_vec`, which are
iterated using `req_iterator`. For each segment, it accesses the `struct page`
where the data from the blockdevice will be written.
6. Once that the block is read, the block driver finishes, the control is returned
to the filesystem and the filesystem returns the result to the VSF, which in turns
return the result to the userspace.

Note that to initialize the block driver, the following steps have to be done:

1. Init `blk_mq_tag_set`.
2. Init `request_queue`. Set the functions for `blk_mq_tag_set`.
3. Init `gendisk`. Set the functions for `block_device_operations`.
4. Add the `gendisk` to the block layer.

## struct gendisk

The `struct gendisk` stores information about the disk.

- https://elixir.bootlin.com/linux/v6.6.2/source/include/linux/blkdev.h#L128

```
struct gendisk {
	/*
	 * major/first_minor/minors should not be set by any new driver, the
	 * block core will take care of allocating them automatically.
	 */
	int major;
	int first_minor;
	int minors;

	char disk_name[DISK_NAME_LEN];	/* name of major driver */

	unsigned short events;		/* supported events */
	unsigned short event_flags;	/* flags related to event processing */

	struct xarray part_tbl;
	struct block_device *part0;

	const struct block_device_operations *fops;
	struct request_queue *queue;
	void *private_data;

	struct bio_set bio_split;

	int flags;
	unsigned long state;
#define GD_NEED_PART_SCAN		0
#define GD_READ_ONLY			1
#define GD_DEAD				2
#define GD_NATIVE_CAPACITY		3
#define GD_ADDED			4
#define GD_SUPPRESS_PART_SCAN		5
#define GD_OWNS_QUEUE			6

	struct mutex open_mutex;	/* open/close mutex */
	unsigned open_partitions;	/* number of open partitions */

	struct backing_dev_info	*bdi;
	struct kobject queue_kobj;	/* the queue/ directory */
	struct kobject *slave_dir;
#ifdef CONFIG_BLOCK_HOLDER_DEPRECATED
	struct list_head slave_bdevs;
#endif
	struct timer_rand_state *random;
	atomic_t sync_io;		/* RAID */
	struct disk_events *ev;

#ifdef CONFIG_BLK_DEV_ZONED
	/*
	 * Zoned block device information for request dispatch control.
	 * nr_zones is the total number of zones of the device. This is always
	 * 0 for regular block devices. conv_zones_bitmap is a bitmap of nr_zones
	 * bits which indicates if a zone is conventional (bit set) or
	 * sequential (bit clear). seq_zones_wlock is a bitmap of nr_zones
	 * bits which indicates if a zone is write locked, that is, if a write
	 * request targeting the zone was dispatched.
	 *
	 * Reads of this information must be protected with blk_queue_enter() /
	 * blk_queue_exit(). Modifying this information is only allowed while
	 * no requests are being processed. See also blk_mq_freeze_queue() and
	 * blk_mq_unfreeze_queue().
	 */
	unsigned int		nr_zones;
	unsigned int		max_open_zones;
	unsigned int		max_active_zones;
	unsigned long		*conv_zones_bitmap;
	unsigned long		*seq_zones_wlock;
#endif /* CONFIG_BLK_DEV_ZONED */

#if IS_ENABLED(CONFIG_CDROM)
	struct cdrom_device_info *cdi;
#endif
	int node_id;
	struct badblocks *bb;
	struct lockdep_map lockdep_map;
	u64 diskseq;
	blk_mode_t open_mode;

	/*
	 * Independent sector access ranges. This is always NULL for
	 * devices that do not have multiple independent access ranges.
	 */
	struct blk_independent_access_ranges *ia_ranges;
};
```

## struct block_device_operations

- https://elixir.bootlin.com/linux/v6.6.2/source/include/linux/blkdev.h#L1375

```
struct block_device_operations {
	void (*submit_bio)(struct bio *bio);
	int (*poll_bio)(struct bio *bio, struct io_comp_batch *iob,
			unsigned int flags);
	int (*open)(struct gendisk *disk, blk_mode_t mode);
	void (*release)(struct gendisk *disk);
	int (*ioctl)(struct block_device *bdev, blk_mode_t mode,
			unsigned cmd, unsigned long arg);
	int (*compat_ioctl)(struct block_device *bdev, blk_mode_t mode,
			unsigned cmd, unsigned long arg);
	unsigned int (*check_events) (struct gendisk *disk,
				      unsigned int clearing);
	void (*unlock_native_capacity) (struct gendisk *);
	int (*getgeo)(struct block_device *, struct hd_geometry *);
	int (*set_read_only)(struct block_device *bdev, bool ro);
	void (*free_disk)(struct gendisk *disk);
	/* this callback is with swap_lock and sometimes page table lock held */
	void (*swap_slot_free_notify) (struct block_device *, unsigned long);
	int (*report_zones)(struct gendisk *, sector_t sector,
			unsigned int nr_zones, report_zones_cb cb, void *data);
	char *(*devnode)(struct gendisk *disk, umode_t *mode);
	/* returns the length of the identifier or a negative errno: */
	int (*get_unique_id)(struct gendisk *disk, u8 id[16],
			enum blk_unique_id id_type);
	struct module *owner;
	const struct pr_ops *pr_ops;

	/*
	 * Special callback for probing GPT entry at a given sector.
	 * Needed by Android devices, used by GPT scanner and MMC blk
	 * driver.
	 */
	int (*alternative_gpt_sector)(struct gendisk *disk, sector_t *sector);
};
```

## struct bio_vec

- https://elixir.bootlin.com/linux/v6.6.2/source/include/linux/bvec.h#L31

```
/**
 * struct bio_vec - a contiguous range of physical memory addresses
 * @bv_page:   First page associated with the address range.
 * @bv_len:    Number of bytes in the address range.
 * @bv_offset: Start of the address range relative to the start of @bv_page.
 *
 * The following holds for a bvec if n * PAGE_SIZE < bv_offset + bv_len:
 *
 *   nth_page(@bv_page, n) == @bv_page + n
 *
 * This holds because page_is_mergeable() checks the above property.
 */
struct bio_vec {
	struct page	*bv_page;
	unsigned int	bv_len;
	unsigned int	bv_offset;
};
```
## struct req_iterator

- https://elixir.bootlin.com/linux/v6.6.2/source/include/linux/blk-mq.h#L1002

```
struct req_iterator {
	struct bvec_iter iter;
	struct bio *bio;
};
```
