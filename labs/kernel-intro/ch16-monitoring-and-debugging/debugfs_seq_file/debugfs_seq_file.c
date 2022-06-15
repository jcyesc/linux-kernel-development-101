// SPDX-License-Identifier: GPL-2.0
/*
 * Module that shows how to use the seq_file interface to print virtual files.
 * This module will print a set of registers.
 *
 *    $ sudo tree /sys/kernel/debug/seqsam
 *    /sys/kernel/debug/seqsam
 *    └── file
 *
 *    0 directories, 1 file
 *    $ sudo cat /sys/kernel/debug/seqsam/file
 *    x0 = 0xDEADBEEF0
 *    x1 = 0xDEADBEEF1
 *    x2 = 0xDEADBEEF2
 *    x3 = 0xDEADBEEF3
 *    x4 = 0xDEADBEEF4
 *    x5 = 0xDEADBEEF5
 *    x6 = 0xDEADBEEF6
 *    x7 = 0xDEADBEEF7
 *    x8 = 0xDEADBEEF8
 *    x9 = 0xDEADBEEF9
 *   x10 = 0xDEADBEEFA
 *   x11 = 0xDEADBEEFB
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
	{ .name = "x0",  .value = 0xDEADBEEF0 },
	{ .name = "x1",  .value = 0xDEADBEEF1 },
	{ .name = "x2",  .value = 0xDEADBEEF2 },
	{ .name = "x3",  .value = 0xDEADBEEF3 },
	{ .name = "x4",  .value = 0xDEADBEEF4 },
	{ .name = "x5",  .value = 0xDEADBEEF5 },
	{ .name = "x6",  .value = 0xDEADBEEF6 },
	{ .name = "x7",  .value = 0xDEADBEEF7 },
	{ .name = "x8",  .value = 0xDEADBEEF8 },
	{ .name = "x9",  .value = 0xDEADBEEF9 },
	{ .name = "x10", .value = 0xDEADBEEFa },
	{ .name = "x11", .value = 0xDEADBEEFb },
};

static void *debugfs_seq_start(struct seq_file *s, loff_t *pos)
{
	pr_info("%s *pos=%lld", __func__, *pos);
	if (*pos >= REGS_NR)
		return NULL;

	return regs + *pos;
}

static void *debugfs_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	pr_info("%s *pos=%lld", __func__, *pos);

	(*pos)++;
	if (*pos >= REGS_NR)
		return NULL;

	return regs + *pos;
}

static int debugfs_seq_show(struct seq_file *s, void *v)
{
	struct reg *r = (struct reg *) v;

	pr_info("%s executing", __func__==);
	seq_printf(s, "%5s = 0x%9llX\n", r->name, r->value);

	return 0;
}

static void debugfs_seq_stop(struct seq_file *s, void *v)
{
	pr_info("%s executing", __func__);
}

static const struct seq_operations seq_ops = {
	.start = debugfs_seq_start,
	.next  = debugfs_seq_next,
	.show  = debugfs_seq_show,
	.stop  = debugfs_seq_stop,
};

static int debugfs_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &seq_ops);
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = debugfs_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

static struct dentry *seqsam;

static int __init debugfs_seq_file_init(void)
{
	pr_info("Init module");

	seqsam = debugfs_create_dir("seqsam", NULL);
	if (IS_ERR(seqsam))
		return PTR_ERR(seqsam);

	debugfs_create_file("file", 0444, seqsam, NULL, &fops);

	return 0;
}

static void __exit debugfs_seq_file_exit(void)
{
	pr_info("Exit module");

	debugfs_remove_recursive(seqsam);
}

module_init(debugfs_seq_file_init);
module_exit(debugfs_seq_file_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Shows how to use seq_file interface.");
MODULE_LICENSE("GPL v2");

