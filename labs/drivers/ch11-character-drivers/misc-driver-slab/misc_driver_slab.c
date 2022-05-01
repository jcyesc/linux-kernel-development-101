/*
 * Defines a Miscellaneous driver that shows
 *
 * - How to use the slab framework.
 * - How to use and include header files in a module.
 * - How to define a Miscellaneous driver.
 *
 * To load the driver, run:
 *
 * pi@raspberrypi:~ $ sudo insmod misc_driver_slab.ko
 * pi@raspberrypi:~ $ ls /dev/miscslab101
 * pi@raspberrypi:~ $ sudo chmod 666 /dev/miscslab101
 * pi@raspberrypi:~ $ echo gato > /dev/miscslab101
 * pi@raspberrypi:~ $ cat /dev/miscslab101
 * pi@raspberrypi:~ $ dmesg
 */

#include <linux/device.h>     // cdev utilities
#include <linux/fs.h>       // File operations.
// https://elixir.bootlin.com/linux/v5.15.12/source/include/linux/init.h
#include <linux/init.h>     // Macros for the module.
#include <linux/miscdevice.h>
#include <linux/module.h>   // for modules
#include <linux/printk.h>   // printk, pr_info
#include <linux/slab.h>     // kmalloc, kfree
#include <linux/types.h>    // loff_t is defined here.

#include "generic_fops.h"

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = generic_cdev_read,
	.write = generic_cdev_write,
	.llseek = generic_cdev_llseek,
	.open = generic_cdev_open,
	.release = generic_cdev_release,
};

static struct miscdevice misc_slab_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = MISC_SLAB_NAME,
	.fops = &fops,
};

static int __init cdriver_init(void)
{
	// Note: In prod drivers, we must check if kmalloc() was successful.
	if (OBJ_SIZE > (1024 * PAGE_SIZE)) {
		pr_err("obj_size is too large. The limit is 1024 pages.\n");
		return -1;
	}

	/*
	 * kmem_cache_create - Create a cache.
	 * @name: A string which is used in /proc/slabinfo to identify this cache.
	 * @size: The size of objects to be created in this cache.
	 * @align: The required alignment for the objects.
	 * @flags: SLAB flags
	 * @ctor: A constructor for the objects.
	 */
	objs_cache = kmem_cache_create(
		"objscache", OBJ_SIZE, 0, SLAB_HWCACHE_ALIGN, NULL);

	if (!objs_cache) {
		pr_err("kmem_cache_create couldn't create the objs_cache.\n");
		return -ENOMEM;
	}

	if (misc_register(&misc_slab_dev)) {
		pr_err("cdriver_init() failed to register the %s\n",
        	MISC_SLAB_NAME);
		kmem_cache_destroy(objs_cache);
		return -EBUSY;
	}

	misc_dev = misc_slab_dev.this_device;
	dev_info(misc_dev, "cdriver_init() succeedded in registering device %s\n",
		MISC_SLAB_NAME);

	return 0;
}

static void __exit cdriver_exit(void)
{
    dev_info(misc_dev, "cdriver_exit() is unregistering device %s\n", MISC_SLAB_NAME);

    misc_deregister(&misc_slab_dev);
    kmem_cache_destroy(objs_cache);
}

module_init(cdriver_init);
module_exit(cdriver_exit);

MODULE_AUTHOR("John");
MODULE_DESCRIPTION("Misc device, llseek() and slab usage example");
MODULE_LICENSE("GPL v2");

