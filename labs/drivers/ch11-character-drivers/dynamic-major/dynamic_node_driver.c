/*
 * This driver shows:
 *
 * - how to create a dynamic node using alloc_chrdev_region(). This means that
 *   it is not necessary to pick a Major and Minor number.
 * - how to populate file_operations.
 * - how to access file and inode information.
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


#define DYNA_CDEV_NAME "dynamicnode101"

/*
 * The @ramdisk will be shared for all user-space apps.
 */
static char *ramdisk;
#define RAMDISK_SIZE ((size_t) (2 * PAGE_SIZE))
// Stores the number of bytes that have been written.
static size_t ramdisk_usage = 0;

static dev_t dyn_dev_num;
static const unsigned int count_cdevs = 1;
static struct cdev *char_cdev;


static int cdev_open(struct inode *inode, struct file *file)
{
	dev_t dev_num;
	dev_num = inode->i_rdev;

	pr_info("cdev_open(): Opening device: %s\n", DYNA_CDEV_NAME);
	pr_info("cdev_open(): Opening device: MAJOR %d MINOR %d\n",
			MAJOR(dev_num), MINOR(dev_num));

	return 0;
}

static int cdev_release(struct inode *inode, struct file *file)
{
	pr_info("cdev_release(): Releasing device: %s\n", DYNA_CDEV_NAME);
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
	pr_info("sam_cdev_read(): Starts lbuf=%d, *ppos=%d, *ramdisk_usage=%d\n",
			(int)lbuf, (int)*ppos, (int)ramdisk_usage);

	return simple_read_from_buffer(buf, lbuf, ppos, ramdisk, ramdisk_usage);
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
cdev_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos)
{
    pr_info("cdev_write(): Starts lbuf=%d, *ppos=%d\n",
        (int)lbuf, (int)*ppos);

    ramdisk_usage = simple_write_to_buffer(
        ramdisk, RAMDISK_SIZE, ppos, buf, lbuf);

    return ramdisk_usage;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = sam_cdev_read,
	.write = cdev_write,
	.open = cdev_open,
	.release = cdev_release,
};

static int __init cdriver_init(void)
{
	int ret;
	int baseminor = 0;
	// Note: In prod drivers, we must check if kmall() was successful.
	ramdisk = kmalloc(RAMDISK_SIZE, GFP_KERNEL);

	/*
	 * * Allocates a range of char device numbers.  The major number will be
	 * chosen dynamically, and returned (along with the first minor number)
	 * in @dev.  Returns zero or a negative error code.
	 */
	ret = alloc_chrdev_region(&dyn_dev_num, baseminor, count_cdevs, DYNA_CDEV_NAME);
	if (ret < 0) {
		pr_err("Failed to allocate character device region \n");
		return -1;
	}

	/* Allocate the cdev structure.. */
	char_cdev = cdev_alloc();
	if (!char_cdev) {
		pr_err("cdev_alloc() failed for %s\n", DYNA_CDEV_NAME);
		unregister_chrdev_region(dyn_dev_num, count_cdevs);
		return -1;
	}

	/*
	 * Initializes @char_cdev, remembering @fops, making it ready to add to the
	 * system with cdev_add().
	 */
	cdev_init(char_cdev, &fops);

	/* Add a char device to the system */
	ret = cdev_add(char_cdev, dyn_dev_num, count_cdevs);
	if (ret < 0) {
		pr_err("cdev_add() failed for %s\n", DYNA_CDEV_NAME);
		cdev_del(char_cdev);
		unregister_chrdev_region(dyn_dev_num, count_cdevs);
	}

	pr_info("cdriver_init(): Character driver %s was registered successfully!\n",
			DYNA_CDEV_NAME);
	pr_info("cdriver_init(): MAJOR %d MINOR %d\n",
			MAJOR(dyn_dev_num), MINOR(dyn_dev_num));
	return 0;
}

static void __exit cdriver_exit(void)
{
	if (char_cdev)
		cdev_del(char_cdev);

	unregister_chrdev_region(dyn_dev_num, count_cdevs);
	kfree(ramdisk);
	pr_info("Character driver %s was unregistered successfully\n", DYNA_CDEV_NAME);
}

module_init(cdriver_init);
module_exit(cdriver_exit);

MODULE_AUTHOR("John");
MODULE_LICENSE("GPL v2");


