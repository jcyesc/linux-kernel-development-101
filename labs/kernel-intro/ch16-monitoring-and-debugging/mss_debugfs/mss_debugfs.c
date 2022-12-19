// SPDX-License-Identifier: GPL-2.0
/*
 * Module to display registers of the Memory Subsystem (MSS). This module
 * will assume that there are 3 memory controllers and every memory controller
 * controls 4 Low-Power Double Data Rate (LPDDR).
 *
 * The directory structure will look like:
 *
 *     $ tree /sys/kernel/debug/mss
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
 *    $ sudo cat /sys/kernel/debug/mss/mc0/lpddr4-1
 *    id       0x00000001
 *    status   0x00000002
 *    ecc      0x00000003
 *    cnt      0x00000004
 *    speed    0x00000005
 *    $ sudo cat /sys/kernel/debug/mss/mc2/lpddr4-0
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
	char buffer[MC_REGS_PRINT_SIZE];
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
static struct dentry *mss;

static inline void populate_regs(struct mc_struct *mc)
{
	int nbytes;
	int i;
	int offset;

	pr_info("Populate regs and buffer for mc->lpddr_id=%d\n", mc->lpddr_id);

	for (i = 0; i < MC_REGS_PER_LPDDR4_NR; i++)
		mc->regs[i] = mc->lpddr_id + i;

	offset = 0;
	nbytes = 0;
	for (i = 0; i < MC_REGS_PER_LPDDR4_NR; i++) {
		nbytes = snprintf(mc->buffer + offset, MC_REG_PRINT_SIZE, "%-10s 0x%.8x\n",
			reg_names[i], mc->regs[i]);
		offset += nbytes;
	}

	WARN(strlen(mc->buffer) >= MC_REGS_PRINT_SIZE,
		"strlen(mc->buffer) should be less than %d", MC_REGS_PRINT_SIZE);

	pr_info("Size of buffer %lu\n", strlen(mc->buffer));
}

static int mss_mc_open(struct inode *inode, struct file *file)
{
	int *lpddr_id = (int *)inode->i_private;
	struct mc_struct *mc;

	mc = kzalloc(sizeof(*mc), GFP_KERNEL);
	if (!mc)
		return -ENOMEM;

	mc->lpddr_id = *lpddr_id;
	populate_regs(mc);
	file->private_data = (void *)mc;

	return 0;
}

static int mss_mc_release(struct inode *inode, struct file *file)
{
	struct mc_struct *mc = file->private_data;

	if (!mc) {
		WARN(!mc, "mc shouldn't be NULL\n");
		return 0;
	}

	kfree(mc);

	return 0;
}

static ssize_t mss_mc_read(
	struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
	struct mc_struct *mc = (struct mc_struct *)file->private_data;

	return simple_read_from_buffer(buf, lbuf, ppos, mc->buffer, strlen(mc->buffer));
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = mss_mc_open,
	.release = mss_mc_release,
	.read = mss_mc_read,
};

static inline void create_mc_lpddr4_subdir(char *dirname, int idx)
{
	struct dentry *mc;

	mc = debugfs_create_dir(dirname, mss);
	debugfs_create_file("lpddr4-0", 0444, mc, &lpddr_ids[idx++], &fops);
	debugfs_create_file("lpddr4-1", 0444, mc, &lpddr_ids[idx++], &fops);
	debugfs_create_file("lpddr4-2", 0444, mc, &lpddr_ids[idx++], &fops);
	debugfs_create_file("lpddr4-3", 0444, mc, &lpddr_ids[idx++], &fops);
}

static int __init mss_debugfs_init(void)
{
	int i;

	mss = debugfs_create_dir("mss", NULL);
	if (IS_ERR(mss))
		return PTR_ERR(mss);

	for (i = 0; i < MC_LPPDR4_NR; i++)
		lpddr_ids[i] = MC0_LPDDR4_0 + i;

	create_mc_lpddr4_subdir("mc0", MC0_LPDDR4_0);
	create_mc_lpddr4_subdir("mc1", MC1_LPDDR4_0);
	create_mc_lpddr4_subdir("mc2", MC2_LPDDR4_0);

	pr_info("Module loaded successfully!");

	return 0;
}

static void __exit mss_debugfs_exit(void)
{
	debugfs_remove_recursive(mss);
}

module_init(mss_debugfs_init);
module_exit(mss_debugfs_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Display memory controller information");
MODULE_LICENSE("GPL v2");

