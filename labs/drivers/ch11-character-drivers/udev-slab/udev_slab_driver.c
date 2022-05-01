/*
 * Driver that device node in the filesystem using udev. This driver also shows:
 *
 * - How to use the slab framework.
 * - How to use and include header files in a module.
 */

#include "generic_fops.h"

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

static struct cdev *udev_cdev;
static struct class *udev_class;
static dev_t udev_dev_num;
static const unsigned int count_cdevs = 1;

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = generic_cdev_read,
	.write = generic_cdev_write,
	.llseek = generic_cdev_llseek,
	.open = generic_cdev_open,
	.release = generic_cdev_release,
};

static int __init udev_init(void)
{
	struct device *parent = NULL;
	void *drvdata = NULL;
	unsigned int baseminor = 0;
	int align = 0;
	void *ctor = NULL;
	int ret = 0;

	// Note: In prod drivers, we must check if kmalloc() was successful.
	if (OBJ_SIZE > (1024 * PAGE_SIZE)) {
		pr_err("obj_size is too large. The limit is 1024 pages.\n");
		ret = -1;
		goto exit;
	}

	objs_cache = kmem_cache_create(
		"objscache", OBJ_SIZE, align, SLAB_HWCACHE_ALIGN, ctor);
	if (!objs_cache) {
		pr_err("kmem_cache_create couldn't create the objs_cache.\n");
		ret = -ENOMEM;
		goto exit;
	}

	if (alloc_chrdev_region(&udev_dev_num, baseminor, count_cdevs, UDEV_SLAB_NAME) < 0) {
		pr_err("Failed to allocate character device region \n");
		ret = -1;
		goto rel_cache;
	}

	udev_cdev = cdev_alloc();
	if (!udev_cdev) {
		pr_err("cdev_alloc() failed");
		ret = -1;
		goto unreg_region;
	}

	cdev_init(udev_cdev, &fops);
	if (cdev_add(udev_cdev, udev_dev_num, count_cdevs) < 0) {
		pr_err("cdev_add() failed\n");
		ret = -1;
		goto dev_del;
	}

	udev_class = class_create(THIS_MODULE, "udevslabclass");
	if (IS_ERR(udev_class)) {
		pr_err("class_create() failed\n");
		ret = -1;
		goto dev_del;
	}

	/*
	 * Creates a device and registers it with sysfs
	 */
	udev_device = device_create(udev_class, parent, udev_dev_num, drvdata, "%s", UDEV_SLAB_NAME);
	if (IS_ERR(udev_device)) {
		pr_err("device_create() failed\n");
		ret = -1;
		goto dest_class;
	}

	dev_info(udev_device, "udev_device was registered successfully.");
	dev_info(udev_device, "MAJOR=%d, MINOR=%d\n", MAJOR(udev_dev_num), MINOR(udev_dev_num));

	goto exit;

	dest_class:
		class_destroy(udev_class);

	dev_del:
		cdev_del(udev_cdev);

	unreg_region:
		unregister_chrdev_region(udev_dev_num, count_cdevs);

	rel_cache:
		kmem_cache_destroy(objs_cache);

	exit:
		return ret;
}

static void __exit udev_exit(void)
{
	dev_info(udev_device, "Unregistering device\n");
	device_destroy(udev_class, udev_dev_num);
	class_destroy(udev_class);
	cdev_del(udev_cdev);
	unregister_chrdev_region(udev_dev_num, count_cdevs);
	kmem_cache_destroy(objs_cache);
}

module_init(udev_init);
module_exit(udev_exit);

MODULE_AUTHOR("John");
MODULE_DESCRIPTION("Use udev to register the device, class and create the device node in sysfs.");
MODULE_LICENSE("GPL v2");




