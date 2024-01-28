// SPDX-License-Identifier: GPL-2.0
/*
 * Driver to send read/write request using a struct bio.
 *
 * This driver will send:
 *
 * - write bio request with 4 segments of 512 bytes.
 * - read bio request with 1 segment of 2024 bytes.
 *
 * This driver works in 5.15.139 and 6.6.2 kernels.
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

#define MSG_SEG_1 "This is the first message :)"
#define MSG_SEG_2 "Second segment"
#define MSG_SEG_3 "The 3rd segment!"
#define MSG_SEG_4 "This is the last segment or the fourth one :)"

static struct block_device *ram_blk_dev;

/*
 * Creates a struct bio object that contains 4 segments to be written to
 * the block device.
 */
int submit_write_request(struct block_device *bdev)
{
	int segments = 4;
	struct page *pages;
	char *buf;
	unsigned int offset = 0;
	int vec_entry_len;
	int order = 2; /* 2 ^ 2 = 4 pages */
	int ret;
	struct bio *bio;
	static const char * const msgs[] = {
		MSG_SEG_1,
		MSG_SEG_2,
		MSG_SEG_3,
		MSG_SEG_4,
	};

	pages = alloc_pages(GFP_NOIO, order);
	if (pages == NULL) {
		pr_info("Failed to allocate pages");
		return -ENOMEM;
	}

	bio = bio_alloc(bdev,
				segments /* number of bvecs to pre-allocate */,
				REQ_OP_WRITE /* operation and flags */,
				GFP_NOIO);
	if (bio == NULL) {
		pr_info("Failed to allocate bio");
		ret = -ENOMEM;
		goto out_pages;
	}

	bio->bi_iter.bi_sector = 0;
	for (int i = 0; i < segments; i++) {
		vec_entry_len = bio_add_page(bio, pages + i, BLK_SECTOR_SIZE, offset);
		buf = kmap_local_page(pages + i);
		memcpy(buf, msgs[i], strlen(msgs[i]));
		kunmap_local(buf);
		pr_info("segment = %d, vec_entry_len = %d", i, vec_entry_len);
	}

	ret = submit_bio_wait(bio);
	if (ret) {
		pr_err("Failed submitting bio request %d", ret);
		goto out_bio;
	}

out_bio:
	bio_put(bio);
out_pages:
	__free_pages(pages, order);

	return ret;
}

/*
 * Creates a struct bio object that request 4 sectors and creates only 1 segment.
 */
int submit_read_request(struct block_device *bdev)
{
	int segments = 1;
	struct page *page;
	char *buf;
	unsigned int offset = 0;
	int vec_entry_len;
	int ret;
	struct bio *bio;

	page = alloc_page(GFP_NOIO);
	if (page == NULL) {
		pr_info("Failed to allocate page");
		return -ENOMEM;
	}

	bio = bio_alloc(bdev,
				segments /* number of bvecs to pre-allocate */,
				REQ_OP_READ /* operation and flags */,
				GFP_NOIO);
	if (bio == NULL) {
		pr_info("Failed to allocate bio");
		ret = -ENOMEM;
		goto out_page;
	}

	bio->bi_iter.bi_sector = 0;

	// Request to read 4 sectors.
	int nr_bytes = BLK_SECTOR_SIZE * 4;

	vec_entry_len = bio_add_page(bio, page, nr_bytes, offset);
	pr_info("segment = %d, vec_entry_len = %d", 1, vec_entry_len);

	ret = submit_bio_wait(bio);
	if (ret) {
		pr_err("Failed submitting bio request %d", ret);
		goto out_bio;
	}

	buf = kmap_local_page(page);
	// Print the first 5 bytes of each message.
	for (int i = 0; i < 4; i++) {
		pr_info("%c %c %c %c %c",
				buf[i * 512],
				buf[1 + i * 512],
				buf[2 + i * 512],
				buf[3 + i * 512],
				buf[4 + i * 512]);
	}
	kunmap_local(buf);

out_bio:
	bio_put(bio);
out_page:
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
	ret = submit_write_request(ram_blk_dev);
	if (ret)
		return ret;

	ret = submit_read_request(ram_blk_dev);
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
