// SPDX-License-Identifier: GPL-2.0
/*
 * Driver to send read/write request using a struct bio.
 */
#define pr_fmt(fmt)  "%s: %s: " fmt, KBUILD_MODNAME, __func__

#include <linux/blkdev.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sprintf.h>
#include <linux/types.h>

/*
 * Block device that will receive the read/write request using the struct bio.
 *
 * The ram_block_driver.c will create the device block.
 *
 * - ch25-filesystems/ram-block-driver/ram_block_driver.c
 */
#define BLKDEV_NAME	"/dev/ramdiskblockdev"
#define BLK_SECTOR_SIZE	512

#define CUSTOM_MESSAGE	"Writing to the File System :)"


static struct block_device *ram_blk_dev;

/*
 * @opf flags and operation.
 */
static int send_bio_request(struct block_device *bdev, blk_opf_t opf)
{
	struct bio *bio = bio_alloc(bdev,
				    1 /* number of bvecs to pre-allocate */,
				    opf /* operation and flags */,
				    GFP_NOIO);
	struct page *page;
	char *buf;
	unsigned int offset = 0;
	int vec_entry_len;
	int ret = 0;

	page = alloc_page(GFP_NOIO);
	if (page == NULL) {
		pr_info("Failed to allocate a page");
		return -ENOMEM;
	}
	buf = kmap_local_page(page);

	// Sector to read or write.
	bio->bi_iter.bi_sector = 0;
	vec_entry_len = bio_add_page(bio, page, BLK_SECTOR_SIZE, offset);
	pr_info("vec_entry_len = %d", vec_entry_len);

	if (opf == REQ_OP_WRITE) {
		// Write to the buffer before the request is submitted.
		//
		// The write request will replace all the content of the sector, even
		// if we write 10 chars. If we want to avoid this behavior, it is
		// necessary to read the desire sector first in the
		// page and then write the new content in the page and submit another
		// bio request.
		//
		// for (int i = 0; i < 20; i++)
		//	pr_info("Before Write: %c ", buf[i]);
		memcpy(buf, CUSTOM_MESSAGE, strlen(CUSTOM_MESSAGE));
	}

	pr_info("Submitting bio %s request", (opf == REQ_OP_READ ? "READ" : "WRITE"));

	ret = submit_bio_wait(bio);
	if (ret) {
		pr_err("Failed submitting bio request %d", ret);
		goto out_err;
	}

	if (opf == REQ_OP_READ) {
		// Read the buffer after the request was submitted.
		// Print only 20 bytes.
		for (int i = 0; i < 20; i++)
			pr_info("READ: %c ", buf[i]);
	}

out_err:
	// Releasing resources.
	pr_info("Releasing resources for bio %s request",
		(opf == REQ_OP_READ ? "READ" : "WRITE"));
	kunmap_local(buf);
	bio_put(bio);
	__free_page(page);

	return ret;
}

static int __init bio_block_init(void)
{
	int ret;

	pr_info("Starting bio block driver");

	/* Open a block device by name */
	ram_blk_dev = blkdev_get_by_path(BLKDEV_NAME,
			BLK_OPEN_READ | BLK_OPEN_WRITE,
			NULL /* void *holder */,
			NULL /* const struct blk_holder_ops *hops */);

	if (IS_ERR(ram_blk_dev)) {
		pr_err("Failed to open block device %s", BLKDEV_NAME);
		return -EINVAL;
	}

	// If the write request comes before the read request, the previous
	// content of the whole sector will be overridden.
	ret = send_bio_request(ram_blk_dev, REQ_OP_WRITE);
	if (ret)
		return ret;

	ret = send_bio_request(ram_blk_dev, REQ_OP_READ);
	if (ret)
		return ret;

	return 0;
}

static void __exit bio_block_exit(void)
{
	pr_info("Exiting bio block driver");

	blkdev_put(ram_blk_dev, NULL /* void *holder */);
}

module_init(bio_block_init);
module_exit(bio_block_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Bio Block Driver");
MODULE_LICENSE("GPL v2");
