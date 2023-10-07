// SPDX-License-Identifier: GPL-2.0
/*
 * Driver to define the file operations for a custom ioctl() call.
 *
 * In order to use the ioctl() system call, it is necessary to define
 * a device node.
 *
 * To install the module use:
 *
 * sudo insmod custom_driver_ioctl.ko
 * sudo chmod 755 /dev/customdev
 *
 * Note:
 * - unlocked_ioctl() - Use it for ARM64 user space.
 * - compat_ioctl() - Use it for ARM32 user space.
 */
#include <linux/device.h>  // Use to create and destroy nodes.
#include <linux/fs.h>      // Defines the file operations prototypes for driver.
#include <linux/init.h>    // Defines module_init(), module_exit().
#include <linux/miscdevice.h>  // Defines the Miscelaneous driver prototypes.
#include <linux/module.h>

#define CUSTOM_DEV_IOCTL "customdev"

static int custom_driver_open(struct inode *inode, struct file *file) {
	pr_info("custom_driver_open() is called.\n");

	return 0;
}

static int custom_driver_close(struct inode *inode, struct file *file) {
	pr_info("custom_driver_close() is called.\n");

	return 0;
}

static long  custom_driver_unlocked_ioctl(
		struct file *fp, unsigned int cmd, unsigned long arg)
{
	pr_info("custom_driver_unlocked_ioctl() cmd=%d, arg=%lu\n", cmd, arg);

	return 0;
}

/*
 * When the Raspberry Pi kernel is compiled for ARM64, the compact_ioctl()
 * function has to be used instead of unlocked_ioctl().
 */
static long  custom_driver_compat_ioctl(
		struct file *fp, unsigned int cmd, unsigned long arg)
{
	pr_info("custom_driver_compat_ioctl() cmd=%d, arg=%lu\n", cmd, arg);

	return 0;
}

static const struct file_operations custom_driver_fops = {
		.owner = THIS_MODULE,
		.open = custom_driver_open,
		.release = custom_driver_close,
		.unlocked_ioctl = custom_driver_unlocked_ioctl,
		.compat_ioctl = custom_driver_compat_ioctl,
};

static struct miscdevice misc_custom_device = {
		.minor = MISC_DYNAMIC_MINOR,
		.name = CUSTOM_DEV_IOCTL,
		.fops = &custom_driver_fops,
};

static int __init custom_driver_init(void)
{
	if (misc_register(&misc_custom_device)) {
		pr_err("Failed to register the misc_custom_device %d.\n",
				misc_custom_device.minor);
		return -EBUSY;
	}

	pr_info("Succeded in registering misc_custom_device %s\n",
			CUSTOM_DEV_IOCTL);
	pr_info(CUSTOM_DEV_IOCTL ": got minor     %i\n", misc_custom_device.minor);
	pr_info(CUSTOM_DEV_IOCTL ": got node name %s\n", misc_custom_device.nodename);

	return 0;
}

static void __exit custom_driver_exit(void)
{
	misc_deregister(&misc_custom_device);
	pr_info("Succeded in unregistering misc_custom_device\n");
}

module_init(custom_driver_init);
module_exit(custom_driver_exit);


MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Custom driver for ioctl() system call");
MODULE_LICENSE("GPL v2");

