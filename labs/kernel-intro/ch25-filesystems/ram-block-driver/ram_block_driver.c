// SPDX-License-Identifier: GPL-2.0
/*
 * Ram Block Driver
 *
 * This block device driver stores the data in memory. It performs the
 * following tasks:
 *
 *  - Create and initialize a blk_mq_tag_set
 *  - Create and initialize a request_queue
 *  - Create and initialize a gendisk.
 *  - Process struct request.
 *  - Allocates memory to store the data using vmalloc.
 *  - Register a block I/O device.
 *
 * The block device will have this structure:
 *
 *   rdbd
 *     |__rdbd1
 *
 * where:
 *
 *   -rdbd represents the block device node.
 *   -rdbd1 represents the first generic disk.
 *
 * This driver works in 5.15.139 and 6.6.2 kernels.
 */
#define pr_fmt(fmt)  "%s: %s: " fmt, KBUILD_MODNAME, __func__

#include <linux/blkdev.h>
#include <linux/blk-mq.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kref.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/numa.h>
#include <linux/part_stat.h>
#include <linux/spinlock.h>

#if KERNEL_VERSION_6_6_2
#include <linux/sprintf.h>
#endif

#include <linux/types.h>
#include <linux/vmalloc.h>

// /sys/block/rdbd1/size
#define NR_SECTORS				131072
#define RAMDISK_SECTOR_SIZE		512

#define RAM_BLKDEV_MAJOR		240
#define RAM_BLKDEV_MINOR		1
#define RAM_BLKDEV_NAME		"rdbd"  // RamDisk Block Device (rdbd)
#define GENDISK_NAME_1			"rdbd1" // Disk 1

static struct ram_block_dev {
	struct gendisk *gd;
	struct blk_mq_tag_set tag_set;
	struct request_queue *queue;

	// Fields that are part of the ramdisk.
	uint8_t *ramdisk;
	size_t size;

	// Fields to protect access to ramdisk.
	// There could be only one reader and writer at a time.
	spinlock_t lock;
} ram_blk_dev;

inline void pr_request(struct request *rq)
{
	switch (rq_data_dir(rq)) {
	case READ:
		pr_info("READ direction request");
		break;
	case WRITE:
		pr_info("WRITE direction request");
		break;
	};

	pr_info("\tblk_rq_pos = __sector =  %llu\n", blk_rq_pos(rq));
	pr_info("\tblk_rq_bytes = __data_len = %d\n", blk_rq_bytes(rq));
	pr_info("\tblk_rq_cur_bytes = %d\n", blk_rq_cur_bytes(rq));

	switch (rq->state) {
	case MQ_RQ_IDLE:
		pr_info("\tmq_rq_state = MQ_RQ_IDLE");
		break;
	case MQ_RQ_IN_FLIGHT:
		pr_info("\tmq_rq_state = MQ_RQ_IN_FLIGHT");
		break;
	case MQ_RQ_COMPLETE:
		pr_info("\tmq_rq_state = MQ_RQ_COMPLETE");
		break;
	};
}

static int exec_request(struct request *rq)
{
	struct bio_vec bvec;
	struct req_iterator iter;
	int i = 0;

	pr_info("Processing segments in bio");

	rq_for_each_segment(bvec, rq, iter) {
		sector_t sector = iter.iter.bi_sector;
		unsigned long buf_offset = bvec.bv_offset;
		unsigned long disk_offset = (sector * RAMDISK_SECTOR_SIZE);
		size_t len = bvec.bv_len;
		char *buffer;
		int dir = bio_data_dir(iter.bio);
		unsigned long flags;

		pr_info("%s Segment %d", (dir == READ ? "READ" : "WRITE"), i++);
		pr_info("   sector:       %lld", sector);
		pr_info("   buf_offset:   %ld", buf_offset);
		pr_info("   disk_offset:  %ld", disk_offset);
		pr_info("   len:          %ld", len);

		if ((disk_offset + len) > ram_blk_dev.size) {
			pr_err("Ramdisk = %ld, disk_offset = %ld, len = %ld",
				ram_blk_dev.size, disk_offset, len);
			return BLK_STS_IOERR;
		}

		buffer = kmap_local_page(bvec.bv_page);

		spin_lock_irqsave(&ram_blk_dev.lock, flags);

		switch (dir) {
		case READ:
			memcpy(buffer + buf_offset, ram_blk_dev.ramdisk + disk_offset, len);
			break;
		case WRITE:
			memcpy(ram_blk_dev.ramdisk + disk_offset, buffer + buf_offset, len);
			break;
		};

		spin_unlock_irqrestore(&ram_blk_dev.lock, flags);

		kunmap_local(buffer);
	}

	return BLK_STS_OK;
}

/**
 * Queue a new request from block IO.
 * To process the request, use these methods:
 *
 * - blk_mq_start_request(): Before starting the request, call this function.
 * - blk_mq_requeue_request(): To send again the request to the queue.
 * - blk_mq_end_request(): When the request is completed, call this function
 *   to notify the upper layers.
 */
static blk_status_t blk_mq_ops_ram_queue_rq(struct blk_mq_hw_ctx *hctx,
									const struct blk_mq_queue_data *bd)
{
	struct request *rq = bd->rq;
	int status;
	struct gendisk *gd = hctx->queue->disk;
	struct ram_block_dev *dev = gd->private_data;

	pr_info("Queuing request for disk '%s' with size %ld bytes\n",
			gd->disk_name, dev->size);

	if (bd->last)
		pr_info("This is the 'last' request in the queue");

	pr_request(rq);

	// Start the request.
	blk_mq_start_request(rq);

	if (blk_rq_is_passthrough(rq)) {
		pr_info("Passthrough request not supported\n");
		blk_mq_end_request(rq, BLK_STS_IOERR);
		goto out;
	}

	status = exec_request(rq);
	blk_mq_end_request(rq, status);

out:
	return BLK_STS_OK;
}

/*
 * Multi-queue operations.
 */
static const struct blk_mq_ops ram_mq_queue_ops = {
	.queue_rq = blk_mq_ops_ram_queue_rq,
};

#if KERNEL_VERSION_6_6_2
/**
 * When the file system is mounted, this method is called.
 */
static int ram_block_open(struct gendisk *disk, blk_mode_t mode)
{
	pr_info("Open disk %s\n", disk->disk_name);

	if (mutex_is_locked(&disk->open_mutex)) {
		pr_info("Disk locked\n");
	} else {
		pr_info("Disk no locked\n");
	}


	return 0;
}

/**
 * When the file system is unmounted, this method is called.
 */
static void ram_block_release(struct gendisk *disk)
{
	pr_info("Release disk %s\n", disk->disk_name);
	if (mutex_is_locked(&disk->open_mutex)) {
		pr_info("Disk locked\n");
	} else {
		pr_info("Disk no locked\n");
	}
}

#elif KERNEL_VERSION_5_15_139

static int ram_block_open(struct block_device *blk, fmode_t mode)
{
	pr_info("Open block device\n");
	return 0;
}

static void ram_block_release(struct gendisk *disk, fmode_t mode)
{
	pr_info("Release disk %s\n", disk->disk_name);
}
#endif

static const struct block_device_operations ram_block_ops = {
	.owner = THIS_MODULE,
	.open = ram_block_open,
	.release = ram_block_release
};

inline int init_blk_mq_tag_set(struct blk_mq_tag_set *tag_set)
{
	tag_set->ops = &ram_mq_queue_ops;
	tag_set->nr_hw_queues = 1;
	tag_set->queue_depth = 16;
	tag_set->numa_node = NUMA_NO_NODE;
	tag_set->cmd_size = 0;
	tag_set->flags = BLK_MQ_F_SHOULD_MERGE;

	pr_info("Before blk_mq_alloc_tag_set()");
	return blk_mq_alloc_tag_set(tag_set);
}

inline int init_request_queue(struct ram_block_dev *dev)
{
	pr_info("Before blk_mq_init_queue()");
	dev->queue = blk_mq_init_queue(&dev->tag_set);
	if (IS_ERR(dev->queue))
		return PTR_ERR(dev->queue);

	// pr_info("Before blk_queue_physical_block_size()");
	// /sys/block/rdbd1/queue/physical_block_size
	blk_queue_physical_block_size(dev->queue, RAMDISK_SECTOR_SIZE);
	// /sys/block/rdbd1/queue/logical_block_size
	blk_queue_logical_block_size(dev->queue, RAMDISK_SECTOR_SIZE);

	// https://elixir.bootlin.com/linux/v6.6.2/source/block/blk-settings.c#L284
	// /sys/block/rdbd1/queue/max_segment_size
	blk_queue_max_segment_size(dev->queue, 512);

	return 0;
}

inline int init_gendisk(struct ram_block_dev *dev)
{
	pr_info("Before blk_mq_alloc_disk()");
	dev->gd = blk_mq_alloc_disk(&dev->tag_set, NULL);
	if (IS_ERR(dev->gd))
		return PTR_ERR(dev->gd);

	dev->queue->disk = dev->gd;
	snprintf(dev->gd->disk_name, DISK_NAME_LEN, GENDISK_NAME_1);

	// major/first_minor/minors should not be set by any new driver, the
	// block core will take care of allocating them automatically.
	//
	// https://elixir.bootlin.com/linux/v6.9.5/source/include/linux/blkdev.h#L134
	dev->gd->major = RAM_BLKDEV_MAJOR;
	dev->gd->first_minor = 1;
	dev->gd->minors = 1;

	dev->gd->flags |= GENHD_FL_NO_PART;
	dev->gd->fops = &ram_block_ops;
	dev->gd->queue = dev->queue;
	dev->gd->private_data = dev;

	// The capacity of the block device is NR_SECTORS * RAMDISK_SECTOR_SIZE
	pr_info("Before set_capacity()");
	set_capacity(dev->gd, NR_SECTORS);

	// This block device is not rotational and it is synchronous.
	blk_queue_flag_set(QUEUE_FLAG_NONROT, dev->gd->queue);
	blk_queue_flag_set(QUEUE_FLAG_SYNCHRONOUS, dev->gd->queue);
	blk_queue_flag_set(QUEUE_FLAG_STABLE_WRITES, dev->gd->queue);

	return 0;
}

static int init_ramdisk(struct ram_block_dev *dev)
{
	dev->size = NR_SECTORS * RAMDISK_SECTOR_SIZE;
	dev->ramdisk = vmalloc(dev->size);
	if (dev->ramdisk == NULL)
		return -ENOMEM;

	return 0;
}

static int setup_block_dev(struct ram_block_dev *dev)
{
	int status;

	spin_lock_init(&dev->lock);

	status = init_ramdisk(dev);
	if (status < 0)
		return status;

	status = init_blk_mq_tag_set(&dev->tag_set);
	if (status < 0)
		goto err_blk_mq_tag;

	status = init_request_queue(dev);
	if (status < 0)
		goto err_blk_init;

	status = init_gendisk(dev);
	if (status < 0)
		goto err_blk_init;

	pr_info("Before add_disk()");
	status = add_disk(dev->gd);
	if (status < 0)
		goto err_blk_init;

	return 0;

err_blk_init:
	blk_mq_free_tag_set(&dev->tag_set);
err_blk_mq_tag:
	vfree(dev->ramdisk);

	return status;
}

static int __init ram_block_init(void)
{
	int status;

	// Register the block device.
	pr_info("Before register_blkdev()");
	status = register_blkdev(RAM_BLKDEV_MAJOR, RAM_BLKDEV_NAME);
	if (status < 0) {
		pr_err("Unable to register '%s'", RAM_BLKDEV_NAME);
		return status;
	}

	status = setup_block_dev(&ram_blk_dev);
	if (status < 0) {
		unregister_blkdev(RAM_BLKDEV_MAJOR, RAM_BLKDEV_NAME);
		return status;
	}

	pr_info("Make sure to create the node:");
	pr_info("    mknod /dev/rdbd b 240 0");

	return 0;
}

static int release_block_device(struct ram_block_dev *dev)
{
	unsigned int clients;

	mutex_lock(&dev->gd->open_mutex);

	clients = disk_openers(dev->gd);
	pr_info("gendisk has [%d] clients(s)!", clients);
	if (clients) {
		mutex_unlock(&dev->gd->open_mutex);
		return -EBUSY;
	}

	mutex_unlock(&dev->gd->open_mutex);

	sync_blockdev(dev->gd->part0);

	pr_info("Deleting gendisk");
	del_gendisk(dev->gd);

	pr_info("Destroying queue");
#if KERNEL_VERSION_6_6_2
	blk_mq_destroy_queue(dev->queue);
#elif KERNEL_VERSION_5_15_139
	blk_cleanup_queue(dev->queue);
#endif

	pr_info("Freeing tag_set");
	blk_mq_free_tag_set(&dev->tag_set);

	pr_info("Freeing ramdisk");
	vfree(dev->ramdisk);

	pr_info("Unregistering block I/O device");
	unregister_blkdev(RAM_BLKDEV_MAJOR, RAM_BLKDEV_NAME);

	return 0;
}

static void __exit ram_block_exit(void)
{
	pr_info("Exiting ram block driver");
	WARN_ON_ONCE(release_block_device(&ram_blk_dev));
}

module_init(ram_block_init);
module_exit(ram_block_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Ram Disk Block Driver (rdbd)");
MODULE_LICENSE("GPL v2");

