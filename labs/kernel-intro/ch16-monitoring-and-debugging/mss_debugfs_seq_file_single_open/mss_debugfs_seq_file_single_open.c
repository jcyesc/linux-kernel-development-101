// SPDX-License-Identifier: GPL-2.0
/*
 * Module to display registers of the Memory Subsystem (MSS). This module
 * will assume that there are 3 memory controllers and every memory controller
 * controls 4 Low-Power Double Data Rate (LPDDR).
 *
 * The directory structure will look like:
 *
 *     $ sudo tree /sys/kernel/debug/mssdbg
 *     ├── mc0
 *     │   ├── lpddr-0
 *     │   ├── lpddr-1
 *     │   ├── lpddr-2
 *     │   └── lpddr-3
 *     ├── mc1
 *     │   ├── lpddr-0
 *     │   ├── lpddr-1
 *     │   ├── lpddr-2
 *     │   └── lpddr-3
 *     └── mc2
 *         ├── lpddr-0
 *         ├── lpddr-1
 *         ├── lpddr-2
 *         └── lpddr-3
 *
 *    $ sudo cat /sys/kernel/debug/mssdbg/mc0/lpddr4-1
 *    id       0x00000001
 *    status   0x00000002
 *    ecc      0x00000003
 *    cnt      0x00000004
 *    speed    0x00000005
 *    $ sudo cat /sys/kernel/debug/mssdbg/mc2/lpddr4-0
 *    id       0x00000008
 *    status   0x00000009
 *    ecc      0x0000000a
 *    cnt      0x0000000b
 *    speed    0x0000000c
 *
 * To mount the debugfs use:
 *
 *    mount -t debugfs none /sys/kernel/debug
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

/*
 * Defines the ids for the different LPDDRs
 */
enum {
	MC0_LPDDR4_0 = 0,
	MC0_LPDDR4_1,
	MC0_LPDDR4_2,
	MC0_LPDDR4_3,
	MC1_LPDDR4_0,
	MC1_LPDDR4_1,
	MC1_LPDDR4_2,
	MC1_LPDDR4_3,
	MC2_LPDDR4_0,
	MC2_LPDDR4_1,
	MC2_LPDDR4_2,
	MC2_LPDDR4_3,

	MC_LPPDR4_NR,
};

#define MC_REGS_PER_LPDDR4_NR 5
#define MC_REG_PRINT_SIZE 24
#define MC_REGS_PRINT_SIZE (MC_REG_PRINT_SIZE * MC_REGS_PER_LPDDR4_NR)

static int lpddr_ids[MC_LPPDR4_NR];

struct mc_struct {
	u32 regs[MC_REGS_PER_LPDDR4_NR];
	int lpddr_id;
};

static char *reg_names[MC_REGS_PER_LPDDR4_NR] = {
	"id",
	"status",
	"ecc",
	"cnt",
	"speed",
};

/*
 * Root directory of the memory subsystem.
 */
static struct dentry *mssdbg;

static inline void populate_regs(struct mc_struct *mc)
{
	int i;

	pr_info("Populate regs for mc->lpddr_id=%d\n", mc->lpddr_id);

	for (i = 0; i < MC_REGS_PER_LPDDR4_NR; i++)
		mc->regs[i] = mc->lpddr_id + i;
}

static int debugfs_single_open(struct seq_file *s, void *v)
{
	int i;
	struct mc_struct *mc;
	int *lpddr_id = (int *)s->private;

	mc = kzalloc(sizeof(*mc), GFP_KERNEL);
	if (!mc)
		return -ENOMEM;

	mc->lpddr_id = *lpddr_id;
	populate_regs(mc);

	pr_info("%s is printing regs for lpddr = %d", __func__, *lpddr_id);

	for (i = 0; i < MC_REGS_PER_LPDDR4_NR; i++)
		seq_printf(s, "%-10s 0x%.8x\n", reg_names[i], mc->regs[i]);

	kfree(mc);

	return 0;
}

#define MAX_VALUE_SIZE 10

struct user_value {
	int lpddr_id;
	char input[MAX_VALUE_SIZE];
	int value;
};

static int mss_mc_open(struct inode *inode, struct file *file)
{
	int *lpddr_id = (int *)inode->i_private;
	struct user_value *uv = kzalloc(sizeof(*uv), GFP_KERNEL);

	uv->lpddr_id = *lpddr_id;

	pr_info("%s: Opening file for lpddr = %d", __func__, uv->lpddr_id);

	return single_open(file, debugfs_single_open, uv);
}

/*
 * When the user writes anything in the debugfs file, the write function is
 * called. For example:
 *
 * echo 1 > /sys/kernel/debug/mssdbg/mc0/lpddr4-1
 */
static ssize_t
mss_mc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)

{
	int num_bytes_written;
	struct seq_file *sf = file->private_data;
	struct user_value *uv = (struct user_value *)(sf->private);
	int lpddr_id = uv->lpddr_id;
	int ret;
	int i;

	pr_info("%s: Starts lpddr = %d, count=%d, *ppos=%d buf=%s",
			__func__, lpddr_id, (int)count, (int)*ppos, buf);

	if (count > MAX_VALUE_SIZE - 1) {
		pr_info("%s: Invalid string, expected size %d, found %ld\n",
				__func__, MAX_VALUE_SIZE - 1, count);
		return -EINVAL;
	}

	num_bytes_written = simple_write_to_buffer(
		uv->input, MAX_VALUE_SIZE - 1, ppos, buf, count);
	pr_info("%s: num_bytes_written = %d", __func__, num_bytes_written);

	if (num_bytes_written == count) {
		// We are done copying the user input.
		uv->input[num_bytes_written] = '\0';

		for (i = 0; i < num_bytes_written; i++) {
			pr_info("%s: char input[%d] = %d, char buf[%d] = %d",
					__func__, i, uv->input[i], i, buf[i]);
		}

		ret = kstrtoint(uv->input, 10, &uv->value);
		if (ret)
			return ret;

		pr_info("%s: Success, value passed is %d", __func__, uv->value);
	}

	return num_bytes_written;
}

int mss_mc_release(struct inode *inode, struct file *file)
{
	struct seq_file *sf = file->private_data;
	struct user_value *uv = (struct user_value *)(sf->private);

	pr_info("%s: Releasing struct user_value", __func__);
	kfree(uv);

	return single_release(inode, file);
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = mss_mc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = mss_mc_release,
	.write = mss_mc_write,
};

static inline void create_mc_lpddr4_subdir(char *dirname, int idx)
{
	struct dentry *mc;

	mc = debugfs_create_dir(dirname, mssdbg);
	debugfs_create_file("lpddr4-0", 0644, mc, &lpddr_ids[idx++], &fops);
	debugfs_create_file("lpddr4-1", 0644, mc, &lpddr_ids[idx++], &fops);
	debugfs_create_file("lpddr4-2", 0644, mc, &lpddr_ids[idx++], &fops);
	debugfs_create_file("lpddr4-3", 0644, mc, &lpddr_ids[idx++], &fops);
}

static int __init mss_debugfs_init(void)
{
	int i;

	mssdbg = debugfs_create_dir("mssdbg", NULL);
	if (IS_ERR(mssdbg))
		return PTR_ERR(mssdbg);

	for (i = 0; i < MC_LPPDR4_NR; i++)
		lpddr_ids[i] = MC0_LPDDR4_0 + i;

	create_mc_lpddr4_subdir("mc0", MC0_LPDDR4_0);
	create_mc_lpddr4_subdir("mc1", MC1_LPDDR4_0);
	create_mc_lpddr4_subdir("mc2", MC2_LPDDR4_0);

	pr_info("Module loaded successfully. See /sys/kernel/debug/mssdbg");

	return 0;
}

static void __exit mss_debugfs_exit(void)
{
	debugfs_remove_recursive(mssdbg);
}

module_init(mss_debugfs_init);
module_exit(mss_debugfs_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Display memory controller information using the seq_file interface");
MODULE_LICENSE("GPL v2");

