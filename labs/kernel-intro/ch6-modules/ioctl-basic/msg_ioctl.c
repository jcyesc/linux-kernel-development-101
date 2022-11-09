// SPDX-License-Identifier: GPL-2.0
/**
 * Driver that set/get greeting message and prints the message in the logs. It
 * uses the ioctl file operation to achieve this.
 *
 * To install the module use:
 *
 * sudo insmod msg_ioctl.ko
 * sudo chmod 755 /dev/msgdev
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": %s() " fmt, __func__

#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#include "msg_lib.h"

#define MSG_DEV "messagedev"

static struct content contnt = {
	.msg = "Default message :)",
};

static int msg_driver_open(struct inode *inode, struct file *file)
{
	pr_info("Opening the device node.");

	return 0;
}

static int msg_driver_close(struct inode *inode, struct file *file)
{
	pr_info("Closing the device node.");

	return 0;
}

static long msg_driver_unlocked_ioctl(
	struct file *file, unsigned int cmd, unsigned long arg)
{
	pr_info("ioctl cmd=%d, arg=%lu", cmd, arg);

	return 0;
}

/**
 * When the Raspberry Pi kernel is compiled for ARM64, the compact_ioctl()
 * function has to be used instead of unlocked_ioctl().
 */
static long msg_driver_compact_ioctl(
	struct file *file, unsigned int cmd, unsigned long arg)
{
	int res;
	int size;

	pr_info("ioctl cmd=%d, arg=%lu", cmd, arg);

	switch (cmd) {
	case MSG_PRINT_IOC:
		pr_info("MSG_PRINT_IOC [%s]", contnt.msg);
		break;
	case MSG_SET_IOC:
		pr_info("MSG_SET_IOC");
		size = _IOC_SIZE(MSG_SET_IOC);
		res = copy_from_user(&contnt, (void __user *) arg, size);
		if (res) {
			pr_info("Error copying from user [%d]\n", res);
			return -EINVAL;
		}
		break;
	case MSG_GET_IOC:
		pr_info("MSG_GET_IOC");
		size = _IOC_SIZE(MSG_GET_IOC);
		res = copy_to_user((void __user *) arg, &contnt, size);
		if (res) {
			pr_info("Error copying to user [%d]\n", res);
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static const struct file_operations msg_fops = {
	.owner = THIS_MODULE,
	.open = msg_driver_open,
	.release = msg_driver_close,
	.compat_ioctl = msg_driver_compact_ioctl,
	.unlocked_ioctl = msg_driver_unlocked_ioctl,
};

static struct miscdevice msg_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = MSG_DEV,
	.fops = &msg_fops,
};

static int __init msg_driver_init(void)
{
	if (misc_register(&msg_misc_device)) {
		pr_err("Failed to register %s\n", MSG_DEV);
		return -EBUSY;
	}

	pr_info(MSG_DEV " was registered successfully\n");
	pr_info(MSG_DEV" got minor       %i\n", msg_misc_device.minor);
	pr_info(MSG_DEV" got node name   %s\n", msg_misc_device.nodename);
	pr_info("The current message is [%s]", contnt.msg);

	return 0;
}

static void __exit msg_driver_exit(void)
{
	misc_deregister(&msg_misc_device);
	pr_info(MSG_DEV " was unregistered successfully\n");
}

module_init(msg_driver_init);
module_exit(msg_driver_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Driver that set/get messages using ioctl().");
MODULE_LICENSE("GPL v2");


