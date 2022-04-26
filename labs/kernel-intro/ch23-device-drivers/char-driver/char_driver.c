/*
 * Character driver example.
 *
 * This char driver allocates a buffer where the user's will read and write.
 */

#include <linux/cdev.h>     // cdev utilities
#include <linux/fs.h>       // File operations.
// https://elixir.bootlin.com/linux/v5.15.12/source/include/linux/init.h
#include <linux/init.h>     // Macros for the module.
#include <linux/module.h>   // for modules
#include <linux/printk.h>   // printk, pr_info
#include <linux/slab.h>     // kmalloc, kfree
#include <linux/types.h>    // loff_t is defined here.
#include <linux/uaccess.h>  // copy_(to, from)_user


#define CHAR_DEV_NAME "chardev101"

static char *ramdisk;
#define RAMDISK_SIZE ((size_t) (2 * PAGE_SIZE))

static dev_t char_dev_num;
static const unsigned int num_of_char_devs = 1;
static int char_dev_major = 500;
static int char_dev_minor = 0;
static struct cdev *char_cdev;


static int char_dev_open(struct inode *inode, struct file *file)
{
	pr_info("char_dev_open(): Opening device: %s\n", CHAR_DEV_NAME);
	return 0;
}

static int char_dev_release(struct inode * inode, struct file *file)
{
	pr_info("char_dev_release(): Releasing device: %s\n", CHAR_DEV_NAME);
	return 0;
}

/**
 * Read from the character device.
 *
 * @file The file to read from. In this example is not used.
 * @buf  The user's buffer that will be populated with the content of the
 *       ramdisk.
 * @lbuf The length of the buffer.
 * @ppos The present position pointer.
 * @return The number of characters that were read.
 */
static ssize_t char_dev_read(
		struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
	int nbytes;
	int bytes_to_read;
	int maxbytes;

	pr_info("char_dev_read(): Starts lbuf=%d, *ppos=%d\n", (int)lbuf, (int)*ppos);

	maxbytes = RAMDISK_SIZE - *ppos;

	bytes_to_read = maxbytes > lbuf ? lbuf : maxbytes;

	if (bytes_to_read == 0)  {
		pr_info("char_dev_read(): The %s reached the end of the file\n", CHAR_DEV_NAME);
		//return -ENOSPC; /* Causes read() to return EOF */
		return 0;
	}

	/* copy_to_user() returns the amount left to copy. */
	nbytes = bytes_to_read - copy_to_user(buf, ramdisk + *ppos, bytes_to_read);
	*ppos += nbytes;
	pr_info("char_dev_read(): Ends nbytes=%d, *ppos=%d\n", nbytes, (int)*ppos);

	return nbytes;
}


/**
 * Writes to the character device.
 *
 * @file The file where the user's buffer will be written. It is not used in
 *       this example.
 * @buf  The user's buffer to write to the character device.
 * @lbuf The length of the buffer.
 * @ppos The present position pointer.
 * @return The number of characters that were written.
 */
static ssize_t
char_dev_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos)
{
    int nbytes;
    int bytes_to_write;
    int maxbytes;

	pr_info("char_dev_write(): Starts lbuf=%d, *ppos=%d\n", (int)lbuf, (int)*ppos);

    maxbytes = RAMDISK_SIZE - *ppos;

    bytes_to_write = maxbytes > lbuf ? lbuf : maxbytes;

    if( bytes_to_write == 0 ) {
		pr_info("char_dev_write(): The %s reached the end of the file\n", CHAR_DEV_NAME);
        // return -ENOSPC; /* Returns EOF at write() */
        return 0;
    }

	/* copy_from_user() returns the amount left to copy. */
    nbytes = bytes_to_write - copy_from_user(ramdisk + *ppos, buf, bytes_to_write);
    *ppos += nbytes;
	pr_info("char_dev_write(): Ends nbytes=%d, *ppos=%d\n", nbytes, (int)*ppos);

    return nbytes;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = char_dev_read,
	.write = char_dev_write,
	.open = char_dev_open,
	.release = char_dev_release,
};

static int __init character_driver_init(void)
{
	ramdisk = kmalloc(RAMDISK_SIZE, GFP_KERNEL);

	/* Create the union of major and minor. */
	char_dev_num = MKDEV(char_dev_major, char_dev_minor);

	/* Register the char device number and name. */
	register_chrdev_region(char_dev_num, num_of_char_devs, CHAR_DEV_NAME);

	/* Allocate and initialize the char device. */
	char_cdev = cdev_alloc();
	cdev_init(char_cdev, &fops);

	/* Associate the char device number with the new cdev. */
	cdev_add(char_cdev, char_dev_num, num_of_char_devs);
	pr_info("Character driver %s was registered successfully!\n", CHAR_DEV_NAME);

	return 0;
}

static void __exit character_driver_exit(void)
{
	cdev_del(char_cdev);
	unregister_chrdev_region(char_dev_num, num_of_char_devs);
	kfree(ramdisk);
	pr_info("Character driver %s was unregistered successfully\n", CHAR_DEV_NAME);
}

module_init(character_driver_init);
module_exit(character_driver_exit);

MODULE_AUTHOR("John");
MODULE_LICENSE("GLP v2");

