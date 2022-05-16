/*
 * Misc driver that prints the memory stats.
 *
 * To read the memory stats, execute:
 *
 * $ sudo chmod +r /dev/memorystats
 * $ cat /dev/memorystats
 *   --------------- Memory Stats ---------------
 *   Examining 242688 pages (num_phys_pages) = 948 MB
 *   Pages with valid PFN's=242688, = 948 MB
 *
 *                        Pages         KB       MB
 *
 *   free           =    148510     594040      580
 *   locked         =         0          0        0
 *   reserved       =      9429      37716       36
 *   swapcache      =         0          0        0
 *   referenced     =     19123      76492       74
 *   slab           =     12898      51592       50
 *   private        =     10087      40348       39
 *   uptodate       =     56180     224720      219
 *   dirty          =      1661       6644        6
 *   active         =     21771      87084       85
 *   writeback      =         0          0        0
 *   mappedtodisk   =     36214     144856      141
 */

#include <linux/cdev.h>     // cdev utilities
#include <linux/fs.h>       // File operations.
// https://elixir.bootlin.com/linux/v5.15.12/source/include/linux/init.h
#include <linux/init.h>     // Macros for the module.
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/module.h>   // for modules
#include <linux/printk.h>   // printk, pr_info
#include <linux/slab.h>     // kmalloc, kfree
#include <linux/string.h>
#include <linux/types.h>    // loff_t is defined here.
#include <linux/uaccess.h>  // copy_(to, from)_user

#define MEMORY_STATS_DEV_NAME "memorystats"
#define RAMDISK_SIZE ((size_t) (2 * PAGE_SIZE))
#define PAGE_STAT(offset, stat, num_pages) snprintf(ramdisk + offset, \
                                   RAMDISK_SIZE, \
                                   "%-15s=%10d %10ld %8ld\n", \
                                   stat, \
                                   num_pages, \
                                   (PAGE_SIZE * num_pages) / 1024, \
                                   (PAGE_SIZE * num_pages) / 1024 / 1024)

static struct device *mem_dev;
static char *ramdisk;
static int memory_stats_size;

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

    dev_info(mem_dev, "populate_memory_stats() is calculating memory stats!\n");

    num_physpages = get_num_physpages();
    for (pfn = 0; pfn < num_physpages; pfn++) {
        /* may be holes due to remapping */
        if (!pfn_valid(pfn))
            continue;
        valid++;
        p = pfn_to_page(pfn);
        /* page_count(page) == 0 is a free page. */
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

    bytes_written = 0;
    bytes_written += snprintf(ramdisk + bytes_written,
            RAMDISK_SIZE,
            "--------------- Memory Stats ---------------\n");

    bytes_written += snprintf(ramdisk + bytes_written,
            RAMDISK_SIZE,
            "Examining %ld pages (num_phys_pages) = %ld MB\n",
            num_physpages,
            num_physpages * PAGE_SIZE / 1024 / 1024);
    bytes_written += snprintf(ramdisk + bytes_written,
            RAMDISK_SIZE,
            "Pages with valid PFN's=%ld, = %ld MB\n",
            valid,
            valid * PAGE_SIZE / 1024 / 1024);

    bytes_written += snprintf(ramdisk + bytes_written,
            RAMDISK_SIZE,
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

    dev_info(mem_dev,
            "populate_memory_stats() %d chars for %s\n",
            bytes_written,
            MEMORY_STATS_DEV_NAME);

    return bytes_written;
}

static int mem_dev_open(struct inode *inode, struct file *file)
{
    dev_info(mem_dev,
             "mem_dev_open(): Opening device: %s\n",
             MEMORY_STATS_DEV_NAME);

    memory_stats_size = populate_memory_stats();

    return 0;
}

static int mem_dev_release(struct inode * inode, struct file *file)
{
    dev_info(mem_dev,
             "mem_dev_release(): Releasing device: %s\n",
             MEMORY_STATS_DEV_NAME);
    return 0;
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
    int bytes_to_read;
    int maxbytes;

    dev_info(mem_dev,
             "mem_dev_read(): Starts lbuf=%d, *ppos=%d\n",
             (int)lbuf,
             (int)*ppos);

    maxbytes = memory_stats_size - *ppos;

    bytes_to_read = maxbytes > lbuf ? lbuf : maxbytes;

    if (bytes_to_read == 0)  {
        dev_info(mem_dev,
                 "mem_dev_read(): The %s reached the end of the file\n",
                 MEMORY_STATS_DEV_NAME);
        //return -ENOSPC; /* Causes read() to return EOF */
        return 0;
    }

    /* copy_to_user() returns the amount left to copy. */
    nbytes = bytes_to_read - copy_to_user(buf, ramdisk + *ppos, bytes_to_read);
    *ppos += nbytes;
    dev_info(mem_dev,
             "mem_dev_read(): Ends nbytes=%d, *ppos=%d\n",
             nbytes,
             (int)*ppos);

    return nbytes;
}

/**
 * Writes to the misc driver. It is not supported.
 *
 * @file The file where the user's buffer will be written. It is not used in
 *       this example.
 * @buf  The user's buffer to write to the character device.
 * @lbuf The length of the buffer.
 * @ppos The present position pointer.
 * @return The number of characters that were written.
 */
static ssize_t
men_dev_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos)
{
    dev_info(mem_dev,
             "mem_dev_write(): Writing to device is not supported: %s\n",
             MEMORY_STATS_DEV_NAME);
    return 0;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = mem_dev_open,
    .release = mem_dev_release,
    .read = mem_dev_read,
    .write = men_dev_write,
};

static struct miscdevice mem_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = MEMORY_STATS_DEV_NAME,
    .fops = &fops,
};

static int __init mem_stats_init(void)
{
    ramdisk = kmalloc(RAMDISK_SIZE, GFP_KERNEL);
    if (!ramdisk) {
        pr_err("mem_stats_init() kmalloc() failed to allocate memory!\n");
        return -EINVAL;
    }

    if (misc_register(&mem_misc_device)) {
        pr_err("mem_stats_init() failed to register %s\n",
               MEMORY_STATS_DEV_NAME);
        kfree(ramdisk);
        return -EBUSY;
    }

    mem_dev = mem_misc_device.this_device;
    dev_info(mem_dev,
             "mem_stats_init() succeedded in registering device %s\n",
             MEMORY_STATS_DEV_NAME);

    return 0;
}

static void __exit mem_stats_exit(void)
{
    dev_info(mem_dev,
             "mem_stats_exit() is unregistering device %s\n",
             MEMORY_STATS_DEV_NAME);
    misc_deregister(&mem_misc_device);
    kfree(ramdisk);
}

module_init(mem_stats_init);
module_exit(mem_stats_exit);

MODULE_AUTHOR("John");
MODULE_DESCRIPTION("Memory stats module");
MODULE_LICENSE("GPL v2");

