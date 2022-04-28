/*
 * This sample character driver shows:
 *
 * - how to populate file_operations.
 * - how to store private information.
 * - how to access file and inode information.
 * - how to use simple_read_from_buffer().
 * - how to use simple_write_to_buffer().
 *
 * Note: This character driver doesn't support concurrency.
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


#define SAM_CDEV_NAME "samcdev101"

static char *ramdisk;
#define RAMDISK_SIZE ((size_t) (2 * PAGE_SIZE))
// Stores the number of bytes that have been written.
static size_t num_of_char_written = 0;

static dev_t sam_cdev_num;
static const unsigned int num_of_sam_cdevs = 1;
static int sam_cdev_major = 500;
static int sam_cdev_minor = 0;
static struct cdev *char_cdev;


static int sam_cdev_open(struct inode *inode, struct file *file)
{
	size_t *private_num;
	dev_t dev_num;

	// Note: In prod drivers, we must check if kmall() was successful.
	private_num = kmalloc(sizeof(*private_num), GFP_KERNEL);

	*private_num = 2333;
	file->private_data = (void *)private_num;

	pr_info("sam_cdev_open(): Opening device: %s with private_data = %d\n",
			SAM_CDEV_NAME, (int)(*private_num));

	dev_num = inode->i_rdev;
	pr_info("sam_cdev_open(): Opening device: MAJOR %d MINOR %d\n",
			MAJOR(dev_num), MINOR(dev_num));

	return 0;
}

static int sam_cdev_release(struct inode *inode, struct file *file)
{
	size_t *private_num;

	private_num = (size_t *)(file->private_data);

	pr_info("sam_cdev_release(): Releasing device: %s with private_data = %d\n",
			SAM_CDEV_NAME, (int)(*private_num));

	kfree(file->private_data);

	return 0;
}

/**
 * Read from the character device.
 *
 * @file The file object that was created by the open() function.
 * @buf  The user's buffer that will be populated with the content of the
 *       ramdisk.
 * @lbuf The length of the buffer.
 * @ppos The present position pointer.
 * @return The number of characters that were read.
 */
static ssize_t sam_cdev_read(
		struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
	pr_info("sam_cdev_read(): Starts lbuf=%d, *ppos=%d, *num_of_char_written=%d\n",
			(int)lbuf, (int)*ppos, (int)num_of_char_written);

	return simple_read_from_buffer(buf, lbuf, ppos, ramdisk, num_of_char_written);
}

/**
 * Writes to the character device.
 *
 * @file The file object that was created by the open() function.
 * @buf  The user's buffer to write to the character device.
 * @lbuf The length of the buffer.
 * @ppos The present position pointer.
 * @return The number of characters that were written.
 */
static ssize_t
sam_cdev_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos)
{
    pr_info("sam_cdev_write(): Starts lbuf=%d, *ppos=%d\n",
        (int)lbuf, (int)*ppos);

    num_of_char_written = simple_write_to_buffer(
        ramdisk, RAMDISK_SIZE, ppos, buf, lbuf);

    return num_of_char_written;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = sam_cdev_read,
	.write = sam_cdev_write,
	.open = sam_cdev_open,
	.release = sam_cdev_release,
};

static int __init character_driver_init(void)
{
	int ret;
	// Note: In prod drivers, we must check if kmall() was successful.
	ramdisk = kmalloc(RAMDISK_SIZE, GFP_KERNEL);

	/* Create the union of major and minor. */
	sam_cdev_num = MKDEV(sam_cdev_major, sam_cdev_minor);

	/* Register the char device number and name. */
	ret = register_chrdev_region(sam_cdev_num, num_of_sam_cdevs, SAM_CDEV_NAME);
	if (ret < 0) {
		pr_err("Failed to register character device region for %s\n",
				SAM_CDEV_NAME);
	}

	/* Allocate and initialize the char device. */
	char_cdev = cdev_alloc();
	if (!char_cdev) {
		pr_err("cdev_alloc() failed for %s\n", SAM_CDEV_NAME);
		unregister_chrdev_region(sam_cdev_num, num_of_sam_cdevs);
		return -1;
	}

	cdev_init(char_cdev, &fops);

	/* Associate the char device number with the new cdev. */
	ret = cdev_add(char_cdev, sam_cdev_num, num_of_sam_cdevs);
	if (ret < 0) {
		pr_err("cdev_add() failed for %s\n", SAM_CDEV_NAME);
		cdev_del(char_cdev);
		unregister_chrdev_region(sam_cdev_num, num_of_sam_cdevs);
	}

	pr_info("Character driver %s was registered successfully!\n", SAM_CDEV_NAME);

	return 0;
}

static void __exit character_driver_exit(void)
{
	if (char_cdev)
		cdev_del(char_cdev);

	unregister_chrdev_region(sam_cdev_num, num_of_sam_cdevs);
	kfree(ramdisk);
	pr_info("Character driver %s was unregistered successfully\n", SAM_CDEV_NAME);
}

module_init(character_driver_init);
module_exit(character_driver_exit);

MODULE_AUTHOR("John");
MODULE_LICENSE("GLP v2");


