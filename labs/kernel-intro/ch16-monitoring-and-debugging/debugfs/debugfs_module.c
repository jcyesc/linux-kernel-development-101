/*
 * Using debugfs.
 *
 * Write a module that creates entries in debugfs.
 *
 * Make sure the debugfs filesystem is mounted and you have support
 * for it in your kernel.
 *
 * First use one of the convenience functions to make just a simple
 * one variable entry under the root debugfs filesystem, of whatever
 * length you desire.
 *
 * Next create your own directory and put one or more entries in it.
 *
 * To mount the debugfs use:
 *
 *    mount -t debugfs none /sys/kernel/debug
 *
 * After the debugfs and the module is loaded, you can try:
 *
 *    sudo cat /sys/kernel/debug/custom_magic_dir/magic_num_file
 *    sudo sh -c 'echo 3578 >  /sys/kernel/debug/custom_magic_dir/magic_num_file'
 *    sudo cat /sys/kernel/debug/custom_magic_dir/year
 *    sudo sh -c 'echo 2022 >  /sys/kernel/debug/custom_magic_dir/year'
 */


#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/uaccess.h>


/*
 * Define the parent directory.
 */
static struct dentry *parent_dir, *magic_num_file;
static u32 magic_num = (u32) 123456;
static u32 year = (u32) 2021;


#define MAX_LENGTH_INPUT 10
static char input_string[MAX_LENGTH_INPUT];

/*
 * Reads the value in {@code magic_num_file} and write it to the user's buffer.
 */
static ssize_t custom_read(
		struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
	int num_bytes;

	// Note: The calculation of num_bytes should be in open() and write() :(.
	num_bytes = sprintf(input_string, "%d\n", magic_num);
	pr_info("%s: parent_dir->d_inode = %p\n", __func__, parent_dir->d_inode);

	return simple_read_from_buffer(buf, lbuf, ppos, input_string, num_bytes);
}

/*
 * Writes the user's given value to {@code magic_num_file}.
 */
static ssize_t custom_write(
		struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos)
{
	int res;
	int num_bytes = lbuf > MAX_LENGTH_INPUT ? MAX_LENGTH_INPUT : lbuf;

	res = copy_from_user(&input_string, buf, num_bytes);
	sscanf(input_string, "%d", &magic_num);
	pr_info("%s: Write num_bytes=%d, magic_num=%d\n",
			__func__, num_bytes, magic_num);

	return num_bytes;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = custom_read,
	.write = custom_write,
};

static int __init debugfs_module_init(void)
{
	pr_info("%s: Loading the module %s at address %p\n",
			KBUILD_MODNAME, KBUILD_MODNAME, init_module);

	// Creates directory at the root of the debugfs filesystem.
	// /sys/kernel/debug/custom_magic_dir
	pr_info("%s: Creating directory /sys/kernel/debug/custom_magic_dir\n",
			KBUILD_MODNAME);
	parent_dir = debugfs_create_dir("custom_magic_dir", NULL);

	// Creates the magic_num_file file.
	// /sys/kernel/debug/custom_magic_dir/magic_num_file
	pr_info("%s: Creating file /sys/kernel/debug/custom_magic_dir/magic_num_file\n",
			KBUILD_MODNAME);
	magic_num_file = debugfs_create_file("magic_num_file", 0644, parent_dir, NULL, &fops);

	// Creates the year variable.
	// /sys/kernel/debug/custom_magic_dir/year
	pr_info("%s: Creating variable /sys/kernel/debug/custom_magic_dir/year\n",
			KBUILD_MODNAME);
	debugfs_create_u32("year", 0644, parent_dir, &year);

	return 0;
}

static void __exit debugfs_module_exit(void)
{
	pr_info("%s: Exiting the module %s at address %p\n",
			KBUILD_MODNAME, KBUILD_MODNAME, cleanup_module);
	debugfs_remove(magic_num_file);
	// When the parent dir is removed, it recursively removes all childen.
	debugfs_remove(parent_dir);
}


module_init(debugfs_module_init);
module_exit(debugfs_module_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Using debugfs filesystem");
MODULE_LICENSE("GPL v2");

