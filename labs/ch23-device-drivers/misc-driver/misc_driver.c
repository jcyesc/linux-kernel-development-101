/*
 * Misc driver example.
 *
 * This misc driver allocates a buffer where the user's will read and write.
 *
 * Note: When misc drivers are used, there is not need to create the device
 *       node with mknod.
 *       All misc drivers share the same major number (10) and can either
 *       request a specific minor number, or have one generated dynamically.
 *
 * To read and write to the misc device:
 *
 * $ sudo chmod 777 /dev/miscdev101
 * $ cat output.txt > /dev/miscdev101
 * $ cat /dev/miscdev101
 */

#include <linux/cdev.h>     // cdev utilities
#include <linux/fs.h>       // File operations.
// https://elixir.bootlin.com/linux/v5.15.12/source/include/linux/init.h
#include <linux/init.h>     // Macros for the module.
#include <linux/miscdevice.h>
#include <linux/module.h>   // for modules
#include <linux/printk.h>   // printk, pr_info
#include <linux/slab.h>     // kmalloc, kfree
#include <linux/types.h>    // loff_t is defined here.
#include <linux/uaccess.h>  // copy_(to, from)_user

#define MISC_DEV_NAME "miscdev101"

static struct device *misc_dev;
static char *ramdisk;
#define RAMDISK_SIZE ((size_t) (2 * PAGE_SIZE))


static int misc_dev_open(struct inode *inode, struct file *file)
{
    dev_info(misc_dev, "misc_dev_open(): Opening device: %s\n", MISC_DEV_NAME);
    return 0;
}

static int misc_dev_release(struct inode * inode, struct file *file)
{
    dev_info(misc_dev, "misc_dev_release(): Releasing device: %s\n", MISC_DEV_NAME);
    return 0;
}

/**
 * Read from the misc device.
 *
 * @file The file to read from. In this example is not used.
 * @buf  The user's buffer that will be populated with the content of the
 *       ramdisk.
 * @lbuf The length of the buffer.
 * @ppos The present position pointer. It is assumed that it starts in ZERO.
 * @return The number of characters that were read.
 */
static ssize_t misc_dev_read(
        struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
    int nbytes;
    int bytes_to_read;
    int maxbytes;

    dev_info(misc_dev, "misc_dev_read(): Starts lbuf=%d, *ppos=%d\n",
    		(int)lbuf, (int)*ppos);

    maxbytes = RAMDISK_SIZE - *ppos;

    bytes_to_read = maxbytes > lbuf ? lbuf : maxbytes;

    if (bytes_to_read == 0)  {
        dev_info(misc_dev, "misc_dev_read(): The %s reached the end of the file\n", MISC_DEV_NAME);
        //return -ENOSPC; /* Causes read() to return EOF */
        return 0;
    }

    /* copy_to_user() returns the amount left to copy. */
    nbytes = bytes_to_read - copy_to_user(buf, ramdisk + *ppos, bytes_to_read);
    *ppos += nbytes;
    dev_info(misc_dev, "misc_dev_read(): Ends nbytes=%d, *ppos=%d\n", nbytes, (int)*ppos);

    return nbytes;
}

/**
 * Writes to the misc driver.
 *
 * @file The file where the user's buffer will be written. It is not used in
 *       this example.
 * @buf  The user's buffer to write to the character device.
 * @lbuf The length of the buffer.
 * @ppos The present position pointer.
 * @return The number of characters that were written.
 */
static ssize_t
misc_dev_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos)
{
    int nbytes;
    int bytes_to_write;
    int maxbytes;

    dev_info(misc_dev, "misc_dev_write(): Starts lbuf=%d, *ppos=%d\n", (int)lbuf, (int)*ppos);

    maxbytes = RAMDISK_SIZE - *ppos;

    bytes_to_write = maxbytes > lbuf ? lbuf : maxbytes;

    if(bytes_to_write == 0) {
        dev_info(misc_dev, "misc_dev_write(): The %s reached the end of the file\n", MISC_DEV_NAME);
        // return -ENOSPC; /* Returns EOF at write() */
        return 0;
    }

    /* copy_from_user() returns the amount left to copy. */
    nbytes = bytes_to_write - copy_from_user(ramdisk + *ppos, buf, bytes_to_write);
    *ppos += nbytes;
    dev_info(misc_dev, "misc_dev_write(): Ends nbytes=%d, *ppos=%d\n", nbytes, (int)*ppos);

    return nbytes;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = misc_dev_read,
    .write = misc_dev_write,
    .open = misc_dev_open,
    .release = misc_dev_release,
};

static struct miscdevice sample_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = MISC_DEV_NAME,
    .fops = &fops,
};

static int __init misc_dev_init(void)
{
    ramdisk = kmalloc(RAMDISK_SIZE, GFP_KERNEL);

    if (!ramdisk) {
        pr_err("misc_dev_init() kmalloc() failed to allocate memory!\n");
        return -EINVAL;
    }

    if (misc_register(&sample_misc_device)) {
        pr_err("misc_dev_init() failed to register the %s\n", MISC_DEV_NAME);
        kfree(ramdisk);
        return -EBUSY;
    }

    misc_dev = sample_misc_device.this_device;
    dev_info(misc_dev, "misc_dev_init() succeedded in registering device %s\n", MISC_DEV_NAME);

    return 0;
}

static void __exit misc_dev_exit(void)
{
    dev_info(misc_dev, "misc_dev_exit() is unregistering device %s\n", MISC_DEV_NAME);
    misc_deregister(&sample_misc_device);
    kfree(ramdisk);
}

module_init(misc_dev_init);
module_exit(misc_dev_exit);

MODULE_AUTHOR("John");
MODULE_DESCRIPTION("Misc device example");
MODULE_LICENSE("GLP v2");

