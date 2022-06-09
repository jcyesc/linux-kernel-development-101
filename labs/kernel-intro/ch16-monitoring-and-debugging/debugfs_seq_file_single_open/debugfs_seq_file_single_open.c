// SPDX-License-Identifier: GPL-2.0
/*
 * Module that shows how to use the seq_file interface and the single_open()
 * function to print virtual files. The example will cover how to:
 *
 * - create virtual file.
 * - use seq_file interface.
 * - single_open() and single_release() function.
 * - set private data for a specific inode.
 *
 * This module will print a set of registers.
 *
 *    $ sudo tree /sys/kernel/debug/singsam
 *    /sys/kernel/debug/singsam
 *    └── file
 *
 *    0 directories, 1 file
 *    $ sudo cat /sys/kernel/debug/singsam/file
 *    x0 = 0xAFEEDBEEF0
 *    x1 = 0xAFEEDBEEF1
 *    x2 = 0xAFEEDBEEF2
 *    x3 = 0xAFEEDBEEF3
 *    x4 = 0xAFEEDBEEF4
 *    x5 = 0xAFEEDBEEF5
 *    x6 = 0xAFEEDBEEF6
 *    x7 = 0xAFEEDBEEF7
 *    x8 = 0xAFEEDBEEF8
 *    x9 = 0xAFEEDBEEF9
 *   x10 = 0xAFEEDBEEFA
 *   x11 = 0xAFEEDBEEFB
 *
 * For more information, see:
 *
 * https://www.kernel.org/doc/html/latest/filesystems/seq_file.html
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/debugfs.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define REGS_NR 12

struct reg {
	char *name;
	u64 value;
};

static struct reg regs[REGS_NR] = {
	{ .name = "x0",  .value = 0xAFEEDBEEF0 },
	{ .name = "x1",  .value = 0xAFEEDBEEF1 },
	{ .name = "x2",  .value = 0xAFEEDBEEF2 },
	{ .name = "x3",  .value = 0xAFEEDBEEF3 },
	{ .name = "x4",  .value = 0xAFEEDBEEF4 },
	{ .name = "x5",  .value = 0xAFEEDBEEF5 },
	{ .name = "x6",  .value = 0xAFEEDBEEF6 },
	{ .name = "x7",  .value = 0xAFEEDBEEF7 },
	{ .name = "x8",  .value = 0xAFEEDBEEF8 },
	{ .name = "x9",  .value = 0xAFEEDBEEF9 },
	{ .name = "x10", .value = 0xAFEEDBEEFa },
	{ .name = "x11", .value = 0xAFEEDBEEFb },
};

static struct dentry *singsam;

static char *greeting = "Hello world";

static int debugfs_single_open(struct seq_file *s, void *v)
{
	int i;
	char *msg = (char *)s->private;

	pr_info("%s executing. msg=%s", __func__, msg);

	for (i = 0; i < REGS_NR; i++)
		seq_printf(s, "%5s = 0x%9llX\n", regs[i].name, regs[i].value);

	return 0;
}

static int debugfs_open(struct inode *inode, struct file *file)
{
	char *msg = (char *)inode->i_private;

	pr_info("%s executing msg=%s", __func__, msg);

	return single_open(file, debugfs_single_open, msg);
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = debugfs_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int __init debugfs_seq_file_init(void)
{
	pr_info("Init module");

	singsam = debugfs_create_dir("singsam", NULL);
	if (IS_ERR(singsam))
		return PTR_ERR(singsam);

	debugfs_create_file("file", 0444, singsam, greeting, &fops);

	return 0;
}

static void __exit debugfs_seq_file_exit(void)
{
	pr_info("Exit module");

	debugfs_remove_recursive(singsam);
}

module_init(debugfs_seq_file_init);
module_exit(debugfs_seq_file_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Shows how to use seq_file interface and single_open function.");
MODULE_LICENSE("GPL v2");

