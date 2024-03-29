// SPDX-License-Identifier: GPL-2.0
/*
 * Misc driver that prints the memory stats.
 *
 * To read the memory stats, execute:
 *
 * $ sudo insmod memory_stats.ko
 * $ sudo mknod -m 666 /dev/memorystats c 501 0
 * $ cat /dev/memorystats
 * --------------- Memory Stats ---------------
 *
 * sizeof(struct mm_struct) = 1064
 * sizeof(struct vm_area_struct) = 152
 *
 * Examining 524288 pages (num_phys_pages) = 2048 MB
 * Pages with valid PFN's=262144, = 1024 MB
 *
 *                      Pages         KB       MB
 *
 * free           =    248344     993376      970
 * locked         =         0          0        0
 * reserved       =      7995      31980       31
 * swapcache      =         0          0        0
 * referenced     =      1991       7964        7
 * slab           =      1624       6496        6
 * private        =       243        972        0
 * uptodate       =      3071      12284       11
 * dirty          =        18         72        0
 * active         =      1967       7868        7
 * writeback      =         0          0        0
 * mappedtodisk   =      2569      10276       10
 */

#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/types.h>

#define MEMORY_STATS_DEV_NAME "memorystats"
#define RAMDISK_SIZE ((size_t) (2 * PAGE_SIZE))
#define PAGE_STAT(offset, stat, num_pages) snprintf(ramdisk + offset, \
			(RAMDISK_SIZE - offset), \
			"%-15s=%10d %10ld %8ld\n", \
			stat, \
			num_pages, \
			(PAGE_SIZE * num_pages) / 1024, \
			(PAGE_SIZE * num_pages) / 1024 / 1024)

static char *ramdisk;
static int stats_dev_major = 501;
static int stats_dev_minor;
static struct cdev *stats_cdev;
static dev_t stats_cdev_num;
static const unsigned int num_cdevs = 1;
static struct mutex ram_mutex;

static int populate_memory_stats(void)
{
	int bytes_written;
	struct page *p;
	unsigned long pfn;  // Page Frame Number.
	unsigned long valid = 0;
	int free = 0;
	int locked = 0;
	int reserved = 0;
	int swapcache = 0;
	int referenced = 0;
	int slab = 0;
	int private = 0;
	int uptodate = 0;
	int dirty = 0;
	int active = 0;
	int writeback = 0;
	int mappedtodisk = 0;
	unsigned long num_physpages;

	num_physpages = get_num_physpages();
	for (pfn = 0; pfn < num_physpages; pfn++) {
		if (!pfn_valid(pfn))
			continue;
		valid++;
		p = pfn_to_page(pfn);
		if (!page_count(p)) {
			free++;
			continue;
		}
		if (PageLocked(p))
			locked++;
		if (PageReserved(p))
			reserved++;
		if (PageSwapCache(p))
			swapcache++;
		if (PageReferenced(p))
			referenced++;
		if (PageSlab(p))
			slab++;
		if (PagePrivate(p))
			private++;
		if (PageUptodate(p))
			uptodate++;
		if (PageDirty(p))
			dirty++;
		if (PageActive(p))
			active++;
		if (PageWriteback(p))
			writeback++;
		if (PageMappedToDisk(p))
			mappedtodisk++;
	}

	// Note: Instead of keeping track of the offsets manually, the seq_file
	// interface and single_open() function can be used.
	bytes_written = 0;
	bytes_written += snprintf(ramdisk + bytes_written,
		RAMDISK_SIZE - bytes_written,
		"--------------- Memory Stats ---------------\n\n");
	bytes_written += snprintf(ramdisk + bytes_written,
		RAMDISK_SIZE - bytes_written,
		"sizeof(struct mm_struct) = %ld\n"
		"sizeof(struct vm_area_struct) = %ld\n\n",
		sizeof(struct mm_struct),
		sizeof(struct vm_area_struct));



	bytes_written += snprintf(ramdisk + bytes_written,
		RAMDISK_SIZE - bytes_written,
		"Examining %ld pages (num_phys_pages) = %ld MB\n",
		num_physpages,
		num_physpages * PAGE_SIZE / 1024 / 1024);
	bytes_written += snprintf(ramdisk + bytes_written,
		RAMDISK_SIZE - bytes_written,
		"Pages with valid PFN's=%ld, = %ld MB\n",
		valid,
		valid * PAGE_SIZE / 1024 / 1024);

	bytes_written += snprintf(ramdisk + bytes_written,
		RAMDISK_SIZE - bytes_written,
		"\n                     Pages         KB       MB\n\n");
	bytes_written += PAGE_STAT(bytes_written, "free", free);
	bytes_written += PAGE_STAT(bytes_written, "locked", locked);
	bytes_written += PAGE_STAT(bytes_written, "reserved", reserved);
	bytes_written += PAGE_STAT(bytes_written, "swapcache", swapcache);
	bytes_written += PAGE_STAT(bytes_written, "referenced", referenced);
	bytes_written += PAGE_STAT(bytes_written, "slab", slab);
	bytes_written += PAGE_STAT(bytes_written, "private", private);
	bytes_written += PAGE_STAT(bytes_written, "uptodate", uptodate);
	bytes_written += PAGE_STAT(bytes_written, "dirty", dirty);
	bytes_written += PAGE_STAT(bytes_written, "active", active);
	bytes_written += PAGE_STAT(bytes_written, "writeback", writeback);
	bytes_written += PAGE_STAT(bytes_written, "mappedtodisk", mappedtodisk);

	return bytes_written;
}

/**
 * Populate the memory stats and write them to the user's buffer.
 *
 * @file The file to read from. In this example is not used.
 * @buf  The user's buffer that will be populated with the memory stats
 * @lbuf The length of the buffer.
 * @ppos The present position pointer.
 * @return The number of characters that were read.
 */
static ssize_t mem_dev_read(
		struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
	int nbytes;
	int stats_size;

	mutex_lock(&ram_mutex);

	// Note: The ramdisk should be populated in the open() method because the
	// read() function can be called several times.
	stats_size = populate_memory_stats();
	nbytes = simple_read_from_buffer(buf, lbuf, ppos, ramdisk, stats_size);

	mutex_unlock(&ram_mutex);

	return nbytes;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = mem_dev_read,
};

static int __init mem_stats_init(void)
{
	int ret;

	mutex_init(&ram_mutex);

	ramdisk = kmalloc(RAMDISK_SIZE, GFP_KERNEL);
	if (!ramdisk)
		return -EINVAL;

	stats_cdev_num = MKDEV(stats_dev_major, stats_dev_minor);

	ret = register_chrdev_region(stats_cdev_num, num_cdevs, MEMORY_STATS_DEV_NAME);
	if (ret < 0) {
		pr_err("Failed to register for %s\n", MEMORY_STATS_DEV_NAME);
		return ret;
	}

	stats_cdev = cdev_alloc();
	if (!stats_cdev) {
		pr_err("Failed to allocate %s\n", MEMORY_STATS_DEV_NAME);
		unregister_chrdev_region(stats_cdev_num, num_cdevs);
		return -EINVAL;
	}

	cdev_init(stats_cdev, &fops);

	ret = cdev_add(stats_cdev, stats_cdev_num, num_cdevs);
	if (ret < 0) {
		pr_err("Failed to add %s\n", MEMORY_STATS_DEV_NAME);
		cdev_del(stats_cdev);
		unregister_chrdev_region(stats_cdev_num, num_cdevs);
		return ret;
	}

	pr_info("%s was registered successfully!\n", MEMORY_STATS_DEV_NAME);

	return 0;
}

static void __exit mem_stats_exit(void)
{
	cdev_del(stats_cdev);
	unregister_chrdev_region(stats_cdev_num, num_cdevs);
	kfree(ramdisk);

	pr_info("%s was unregistered successfully\n", MEMORY_STATS_DEV_NAME);
}

module_init(mem_stats_init);
module_exit(mem_stats_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Memory stats module");
MODULE_LICENSE("GPL v2");

