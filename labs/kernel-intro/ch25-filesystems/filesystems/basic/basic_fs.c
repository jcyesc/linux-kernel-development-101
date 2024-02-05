// SPDX-License-Identifier: GPL-2.0
/*
 * Basic File System
 *
 * This driver defines a basic file system. This driver will:
 *
 *  - Register the file system.
 *  - Fill the super block.
 *  - Define the superblock operations.
 *
 * This driver works in 5.15.139 and 6.6.14 kernels.
 */
#define pr_fmt(fmt) "%s: %s: " fmt, KBUILD_MODNAME, __func__

#include <linux/blkdev.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#if KERNEL_VERSION_6_6_2
#include <linux/pagemap.h>
#include <linux/mnt_idmapping.h>
#elif KERNEL_VERSION_5_15_139
#include <linux/user_namespace.h>
#endif

#define BASIC_FS_BLOCKSIZE		512
#define BASIC_FS_BLOCKSIZE_BITS	9
#define BASIC_FS_MAGIC			0xDEADBEEF

static const struct super_operations basic_fs_ops = {
	.statfs = simple_statfs,
	.drop_inode	= generic_drop_inode,
};

#if KERNEL_VERSION_6_6_2

static int basic_fs_create(struct mnt_idmap *idmap, struct inode *dir,
		struct dentry *dentry, umode_t mode, bool excl);
static int basic_fs_mknod(struct mnt_idmap *idmap, struct inode *dir,
		struct dentry *dentry, umode_t mode, dev_t dev);
static int basic_fs_mkdir(struct mnt_idmap *idmap, struct inode *dir,
		struct dentry *dentry, umode_t mode);

#elif KERNEL_VERSION_5_15_139

static int basic_fs_create(struct user_namespace *user_ns, struct inode *dir,
		struct dentry *dentry, umode_t mode, bool excl);
static int basic_fs_mknod(struct user_namespace *user_ns, struct inode *dir,
		struct dentry *dentry, umode_t mode, dev_t dev);
static int basic_fs_mkdir(struct user_namespace *user_ns, struct inode *dir,
		struct dentry *dentry, umode_t mode);
#endif


static const struct inode_operations basic_fs_dir_inode_operations = {
	.create = basic_fs_create,
	.mknod = basic_fs_mknod,
	.mkdir = basic_fs_mkdir,
	.rmdir = simple_rmdir,
	.rename = simple_rename,
	.lookup = simple_lookup,
	.link = simple_link,
	.unlink = simple_unlink,
};

static const struct file_operations *basic_fs_dir_file_operations =
		&simple_dir_operations;

static const struct inode_operations basic_fs_file_inode_operations = {
	.setattr = simple_setattr,
	.getattr = simple_getattr,
};

#if KERNEL_VERSION_6_6_2

static const struct file_operations basic_fs_file_operations = {
	.read_iter = generic_file_read_iter,
	.write_iter	= generic_file_write_iter,
	.mmap = generic_file_mmap,
	.fsync = noop_fsync,
	.splice_write = iter_file_splice_write,
	.llseek = generic_file_llseek,
};

#elif KERNEL_VERSION_5_15_139

static const struct file_operations basic_fs_file_operations = {
	.read_iter = generic_file_read_iter,
	.write_iter	= generic_file_write_iter,
	.mmap = generic_file_mmap,
	.fsync = noop_fsync,
	.splice_read = generic_file_splice_read,
	.splice_write = iter_file_splice_write,
	.llseek = generic_file_llseek,
};

#endif

static const struct address_space_operations *basicfs_aops = &ram_aops;

struct inode *basic_fs_create_inode(struct super_block *sb,
		const struct inode *dir, int mode)
{
	struct inode *inode = new_inode(sb);
	struct timespec64 curr_time;

	if (!inode)
		return NULL;

	curr_time = current_time(inode);
	inode->i_atime = curr_time;
	inode->i_mtime = curr_time;

#if KERNEL_VERSION_6_6_2
	inode_set_ctime_to_ts(inode, curr_time);
	inode_init_owner(&nop_mnt_idmap, inode, dir, mode);
#elif KERNEL_VERSION_5_15_139
	inode->i_ctime = curr_time;
	inode_init_owner(&init_user_ns, inode, dir, mode);
#endif

	inode->i_ino = get_next_ino();
	inode->i_mapping->a_ops = basicfs_aops;

	if (S_ISDIR(mode)) {
		pr_info("set inode and file operations for a dir");
		inode->i_op = &basic_fs_dir_inode_operations;
		inode->i_fop = basic_fs_dir_file_operations;

		// Directory nlink count must be incremented because of the (.) directory.
		inc_nlink(inode);
	} else if (S_ISREG(mode)) {
		pr_info("set inode and file operations for regular file");
		inode->i_op = &basic_fs_file_inode_operations;
		inode->i_fop = &basic_fs_file_operations;
	} else {
		pr_err("Type of file not supported");
	}

	pr_info("inode->i_ino = %lu, inode->i_nlink = %d", inode->i_ino, inode->i_nlink);

	return inode;
}

#if KERNEL_VERSION_6_6_2

static int basic_fs_create(struct mnt_idmap *idmap, struct inode *dir_inode,
		struct dentry *dentry, umode_t mode, bool excl)
{
	pr_info("Creating node");
	return basic_fs_mknod(idmap, dir_inode, dentry, mode | S_IFREG, 0);
}

static int basic_fs_mkdir(struct mnt_idmap *idmap, struct inode *dir_inode,
		struct dentry *dentry, umode_t mode)
{
	int ret;

	pr_info("Making dir");
	ret = basic_fs_mknod(idmap, dir_inode, dentry, mode | S_IFDIR, 0);
	if (ret != 0)
		return ret;

	// inc_nlink(dir_inode);

	return 0;
}

static int basic_fs_mknod(struct mnt_idmap *idmap, struct inode *dir_inode,
		struct dentry *dentry, umode_t mode, dev_t dev)
{
	struct inode *inode = basic_fs_create_inode(dir_inode->i_sb, dir_inode, mode);
	struct timespec64 curr_time;

	pr_info("Making node");
	if (inode == NULL)
		return -ENOSPC;

	d_instantiate(dentry, inode);
	dget(dentry);
	curr_time = current_time(inode);
	dir_inode->i_mtime = curr_time;
	inode_set_ctime_to_ts(dir_inode, curr_time);

	return 0;
}

#elif KERNEL_VERSION_5_15_139

static int basic_fs_create(struct user_namespace *user_ns, struct inode *dir_inode,
		struct dentry *dentry, umode_t mode, bool excl)
{
	pr_info("Creating node");
	return basic_fs_mknod(user_ns, dir_inode, dentry, mode | S_IFREG, 0);
}

static int basic_fs_mkdir(struct user_namespace *user_ns, struct inode *dir_inode,
		struct dentry *dentry, umode_t mode)
{
	int ret;

	pr_info("Making dir");
	ret = basic_fs_mknod(user_ns, dir_inode, dentry, mode | S_IFDIR, 0);
	if (ret != 0)
		return ret;

	inc_nlink(dir_inode);

	return 0;
}

static int basic_fs_mknod(struct user_namespace *user_ns, struct inode *dir_inode,
		struct dentry *dentry, umode_t mode, dev_t dev)
{
	struct inode *inode = basic_fs_create_inode(dir_inode->i_sb, dir_inode, mode);
	struct timespec64 curr_time;

	pr_info("Making node")
;
	if (inode == NULL)
		return -ENOSPC;

	d_instantiate(dentry, inode);
	dget(dentry);
	curr_time = current_time(inode);
	dir_inode->i_mtime = curr_time;
	dir_inode->i_ctime = curr_time;

	return 0;
}

#endif

static int basic_fs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *root_inode = NULL;
	struct inode *dir = NULL;

	pr_info("Filling super block");

	sb->s_maxbytes = MAX_LFS_FILESIZE;
	sb->s_blocksize = BASIC_FS_BLOCKSIZE;
	sb->s_blocksize_bits = BASIC_FS_BLOCKSIZE_BITS;
	sb->s_magic = BASIC_FS_MAGIC;
	sb->s_op = &basic_fs_ops;
	sb->s_flags |= SB_ACTIVE;

	// https://www.gnu.org/software/libc/manual/html_node/Permission-Bits.html
	root_inode = basic_fs_create_inode(sb, dir,
			S_IFDIR |
			(S_IRUSR | S_IWUSR | S_IXUSR) |  // User permissions  7
			(S_IRGRP | S_IXGRP) |            // Group permissions 5
			(S_IROTH | S_IXOTH));            // Other permissions 5

	if (!root_inode)
		return -ENOMEM;

	pr_info("root_inode->i_nlink = %d", root_inode->i_nlink);

	sb->s_root = d_make_root(root_inode);
	if (!sb->s_root)
		goto out_null_root;

	return 0;

out_null_root:
	iput(root_inode);

	return -ENOMEM;
}

struct dentry* basic_fs_mount(struct file_system_type *fs_type,
		int flags, const char *dev_name, void *data)
{
	pr_info("mounting file system");

	/*
	 * There are several functions provided to mount a file system.
	 *
	 * - mount_bdev(): mounts a file system that is stored in a block device.
	 * - mount_single(): mounts a file system that shares the same super block
	 *                   instance.
	 * - mount_nodev(): mounts a file system that is not stored in a block device.
	 * - mount_pseudo(): mounts a pseudo-file systems.
	 */
	return mount_nodev(fs_type, flags, data, basic_fs_fill_super);
}

/*
 * There are three methods that implement the kill_sb() function:
 *
 * kill_block_super(): unmounts a file system that is mounted on a block device.
 * kill_anon_super(): unmounts a file system that generates the files on demand.
 * kill_litter_super(): unmounts a file system that is not mounted on a block device.
 */
static struct file_system_type basic_fs_type = {
	.owner = THIS_MODULE,
	.name = "basicfs",
	.mount = basic_fs_mount,
	.kill_sb = kill_litter_super,
};

static int __init basic_fs_init(void)
{
	int err;

	pr_info("Init the basic filesystem module");
	err = register_filesystem(&basic_fs_type);
	if (err) {
		pr_warn("Failed to register the file system");
		return err;
	}

	return 0;
}

static void __exit basic_fs_exit(void)
{
	pr_info("Exiting the module");
	unregister_filesystem(&basic_fs_type);
}

module_init(basic_fs_init);
module_exit(basic_fs_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Basic file system implementation");
MODULE_LICENSE("GPL v2");
