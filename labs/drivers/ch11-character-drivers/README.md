
# Chapter 11 - Character Drivers

The topics discussed in this chapter are:

- Device nodes
- Major and Minor numbers
- Reserving Major/Minor numbers
- Accessing the device node
- Registering the device
- udev
- `dev_printk()` and more
- `file_operations` structure
- Driver entry points
- The `file` adn `inode` structures
- Miscellaneous character drivers


## Device Nodes

The way that userspace applications communicate with the character and block
devices is through device nodes. A device node is an entry in the filesystem.

We can create device nodes using:

```shell
$ mknod [-m mode] /dev/node_name <type> <major> <minor>

# Example
$ mknode -m 666 /dev/cusdrv c 852 1

```

## Major and Minor numbers

The major and minor numbers are stored in the `dev_t` variable, which it is
32-bits long. 12 bits are for the major number and 20 bits are for the minor
number. Below are some macros used to extract the numbers from `dev_t`.

```c
MAJOR(dev_t dev);
MINOR(dev_t dev);
MKDEV(int major, int minor);
```
## Reserving Major/Minor numbers

To reserve or allocate major/minor numbers, we can use the functions defined
in `include/linux/fs.h`:

```c
extern int alloc_chrdev_region(dev_t *, unsigned, unsigned, const char *);
extern int register_chrdev_region(dev_t, unsigned, const char *);
```

## Accessing the Device Node

In order to access a device, it is necessary to access the device node using
the following user space functions:


```c
int open(const char *pathname, int flags);
int close(int fd);
ssize_t read(int fd, void *buf, size_t count);
ssize write(int fd, const void *buf, size_t count);
int ioctl(int fd, int request, ...);
void *mmap (void *start, size_t length, int prot, int flags, int fd, off_t offset);
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
```

## Registering the device

To register the device, first, we need to allocate memory for `struct cdev`
and then init the structure. The life cycle methods for `cdev` are below:


```c
// Define in /include/linux/cdev.h
struct cdev {
	struct kobject kobj;
	struct module *owner;
	const struct file_operations *ops;
	struct list_head list;
	dev_t dev;
	unsigned int count;
} __randomize_layout;

void cdev_init(struct cdev *, const struct file_operations *);

struct cdev *cdev_alloc(void);

void cdev_put(struct cdev *p);

int cdev_add(struct cdev *, dev_t, unsigned);

void cdev_set_parent(struct cdev *p, struct kobject *kobj);
int cdev_device_add(struct cdev *cdev, struct device *dev);
void cdev_device_del(struct cdev *cdev, struct device *dev);

void cdev_del(struct cdev *);
```

## udev

`udev` is a daemon that monitors a `netlink` socket. It is through this socket
that `udev` receives the instructions to add/remove device nodes. The `u`
in `udev` stands for `user` and indicates that most of the work is done in
user space. `udev` replaces `devfs` and `hotplug` mechanisms.

The three components of `udev` are:

1. The `libudev` library which allows access to information about the devices.
2. The `udevd` daemon (or `systemd-udevd`) that manages the `/dev` directory.
3. The `udevadm` utility for control and diagnostics.


## dev_printk()

When your device driver uses the `struct device` data type, you can use
the macros below to print information. These macros prepend the name of the
device before the messages.

- https://elixir.bootlin.com/linux/v5.17.4/source/include/linux/dev_printk.h#L126

```c
#define dev_printk(level, dev, fmt, ...)				\
	({								\
		dev_printk_index_emit(level, fmt);			\
		_dev_printk(level, dev, fmt, ##__VA_ARGS__);		\
	})

/*
 * #defines for all the dev_<level> macros to prefix with whatever
 * possible use of #define dev_fmt(fmt) ...
 */

#define dev_emerg(dev, fmt, ...) \
	dev_printk_index_wrap(_dev_emerg, KERN_EMERG, dev, dev_fmt(fmt), ##__VA_ARGS__)
#define dev_crit(dev, fmt, ...) \
	dev_printk_index_wrap(_dev_crit, KERN_CRIT, dev, dev_fmt(fmt), ##__VA_ARGS__)
#define dev_alert(dev, fmt, ...) \
	dev_printk_index_wrap(_dev_alert, KERN_ALERT, dev, dev_fmt(fmt), ##__VA_ARGS__)
#define dev_err(dev, fmt, ...) \
	dev_printk_index_wrap(_dev_err, KERN_ERR, dev, dev_fmt(fmt), ##__VA_ARGS__)
#define dev_warn(dev, fmt, ...) \
	dev_printk_index_wrap(_dev_warn, KERN_WARNING, dev, dev_fmt(fmt), ##__VA_ARGS__)
#define dev_notice(dev, fmt, ...) \
	dev_printk_index_wrap(_dev_notice, KERN_NOTICE, dev, dev_fmt(fmt), ##__VA_ARGS__)
#define dev_info(dev, fmt, ...) \
	dev_printk_index_wrap(_dev_info, KERN_INFO, dev, dev_fmt(fmt), ##__VA_ARGS__)
```

## file_operations and inode_operations structures

The `file_operations` that a driver could implement to distpach user space
request are enumerated below:

- https://elixir.bootlin.com/linux/v5.17.4/source/include/linux/fs.h#L1981

```c
struct file_operations {
	struct module *owner;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
	ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
	int (*iopoll)(struct kiocb *kiocb, struct io_comp_batch *,
			unsigned int flags);
	int (*iterate) (struct file *, struct dir_context *);
	int (*iterate_shared) (struct file *, struct dir_context *);
	__poll_t (*poll) (struct file *, struct poll_table_struct *);
	long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
	long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	unsigned long mmap_supported_flags;
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *, fl_owner_t id);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, loff_t, loff_t, int datasync);
	int (*fasync) (int, struct file *, int);
	int (*lock) (struct file *, int, struct file_lock *);
	ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
	unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
	int (*check_flags)(int);
	int (*flock) (struct file *, int, struct file_lock *);
	ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
	ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
	int (*setlease)(struct file *, long, struct file_lock **, void **);
	long (*fallocate)(struct file *file, int mode, loff_t offset,
			  loff_t len);
	void (*show_fdinfo)(struct seq_file *m, struct file *f);
#ifndef CONFIG_MMU
	unsigned (*mmap_capabilities)(struct file *);
#endif
	ssize_t (*copy_file_range)(struct file *, loff_t, struct file *,
			loff_t, size_t, unsigned int);
	loff_t (*remap_file_range)(struct file *file_in, loff_t pos_in,
				   struct file *file_out, loff_t pos_out,
				   loff_t len, unsigned int remap_flags);
	int (*fadvise)(struct file *, loff_t, loff_t, int);
} __randomize_layout;
```

The available `inode_operations` are below:

- https://elixir.bootlin.com/linux/v5.17.4/source/include/linux/fs.h#L2024


```c
struct inode_operations {
	struct dentry * (*lookup) (struct inode *,struct dentry *, unsigned int);
	const char * (*get_link) (struct dentry *, struct inode *, struct delayed_call *);
	int (*permission) (struct user_namespace *, struct inode *, int);
	struct posix_acl * (*get_acl)(struct inode *, int, bool);

	int (*readlink) (struct dentry *, char __user *,int);

	int (*create) (struct user_namespace *, struct inode *,struct dentry *,
		       umode_t, bool);
	int (*link) (struct dentry *,struct inode *,struct dentry *);
	int (*unlink) (struct inode *,struct dentry *);
	int (*symlink) (struct user_namespace *, struct inode *,struct dentry *,
			const char *);
	int (*mkdir) (struct user_namespace *, struct inode *,struct dentry *,
		      umode_t);
	int (*rmdir) (struct inode *,struct dentry *);
	int (*mknod) (struct user_namespace *, struct inode *,struct dentry *,
		      umode_t,dev_t);
	int (*rename) (struct user_namespace *, struct inode *, struct dentry *,
			struct inode *, struct dentry *, unsigned int);
	int (*setattr) (struct user_namespace *, struct dentry *,
			struct iattr *);
	int (*getattr) (struct user_namespace *, const struct path *,
			struct kstat *, u32, unsigned int);
	ssize_t (*listxattr) (struct dentry *, char *, size_t);
	int (*fiemap)(struct inode *, struct fiemap_extent_info *, u64 start,
		      u64 len);
	int (*update_time)(struct inode *, struct timespec64 *, int);
	int (*atomic_open)(struct inode *, struct dentry *,
			   struct file *, unsigned open_flag,
			   umode_t create_mode);
	int (*tmpfile) (struct user_namespace *, struct inode *,
			struct dentry *, umode_t);
	int (*set_acl)(struct user_namespace *, struct inode *,
		       struct posix_acl *, int);
	int (*fileattr_set)(struct user_namespace *mnt_userns,
			    struct dentry *dentry, struct fileattr *fa);
	int (*fileattr_get)(struct dentry *dentry, struct fileattr *fa);
} ____cacheline_aligned;
```

# The `file` and `inode` structures

The `file` and `inode` structures are defined in [include/linux/fs.h].
Most of the functions in `struct file_operations` receive a `struct file`
pointer.

When the `open()` function is called in the device, a `struct file` object
is created automatically and it is released when the `release()` function in
`file_operations` is called. There could be several `file` objects for a
specif device node. The `file` object that is created when `open()` is called,
it is passed to subsequent calls in `file_operatations`, for example: `read()`,
`write()`, `fsync()`, etc.

Some of the relevant fields in `file` are:

- `f_path`: gives information about the directory entry.
- `inode`: the `inode` that represents the device node. There will be only
one pointing to the device node, even if there are multiple `file` objects.


```c
struct file {
	union {
		struct llist_node	fu_llist;
		struct rcu_head 	fu_rcuhead;
	} f_u;
	struct path		f_path;
	struct inode		*f_inode;	/* cached value */
	const struct file_operations	*f_op;

	/*
	 * Protects f_ep, f_flags.
	 * Must not be taken from IRQ context.
	 */
	spinlock_t		f_lock;
	enum rw_hint		f_write_hint;
	atomic_long_t		f_count;
	unsigned int 		f_flags;
	fmode_t			f_mode;
	struct mutex		f_pos_lock;
	loff_t			f_pos;
	struct fown_struct	f_owner;
	const struct cred	*f_cred;
	struct file_ra_state	f_ra;

	u64			f_version;
#ifdef CONFIG_SECURITY
	void			*f_security;
#endif
	/* needed for tty driver, and maybe others */
	void			*private_data;

#ifdef CONFIG_EPOLL
	/* Used by fs/eventpoll.c to link all the hooks to this file */
	struct hlist_head	*f_ep;
#endif /* #ifdef CONFIG_EPOLL */
	struct address_space	*f_mapping;
	errseq_t		f_wb_err;
	errseq_t		f_sb_err; /* for syncfs */
} __randomize_layout
  __attribute__((aligned(4)));	/* lest something weird decides that 2 is OK */
```

For every file and directory in the system, Linux creates `struct inode` that
stores metadata about the file or directory. The metadata contains the information
necessary to find the data blocks for the file or directory.

There are a couple of fields that are useful:

`dev_t	i_rdev`: Contains the major and minor numbers for the device node.
`struct cdev *i_cdev`: Points to the character device structure.


```c
/*
 * Keep mostly read-only and often accessed (especially for
 * the RCU path lookup and 'stat' data) fields at the beginning
 * of the 'struct inode'
 */
struct inode {
	umode_t			i_mode;
	unsigned short		i_opflags;
	kuid_t			i_uid;
	kgid_t			i_gid;
	unsigned int		i_flags;

#ifdef CONFIG_FS_POSIX_ACL
	struct posix_acl	*i_acl;
	struct posix_acl	*i_default_acl;
#endif

	const struct inode_operations	*i_op;
	struct super_block	*i_sb;
	struct address_space	*i_mapping;

#ifdef CONFIG_SECURITY
	void			*i_security;
#endif

	/* Stat data, not accessed from path walking */
	unsigned long		i_ino;
	/*
	 * Filesystems may only read i_nlink directly.  They shall use the
	 * following functions for modification:
	 *
	 *    (set|clear|inc|drop)_nlink
	 *    inode_(inc|dec)_link_count
	 */
	union {
		const unsigned int i_nlink;
		unsigned int __i_nlink;
	};
	dev_t			i_rdev;
	loff_t			i_size;
	struct timespec64	i_atime;
	struct timespec64	i_mtime;
	struct timespec64	i_ctime;
	spinlock_t		i_lock;	/* i_blocks, i_bytes, maybe i_size */
	unsigned short          i_bytes;
	u8			i_blkbits;
	u8			i_write_hint;
	blkcnt_t		i_blocks;

#ifdef __NEED_I_SIZE_ORDERED
	seqcount_t		i_size_seqcount;
#endif

	/* Misc */
	unsigned long		i_state;
	struct rw_semaphore	i_rwsem;

	unsigned long		dirtied_when;	/* jiffies of first dirtying */
	unsigned long		dirtied_time_when;

	struct hlist_node	i_hash;
	struct list_head	i_io_list;	/* backing dev IO list */
#ifdef CONFIG_CGROUP_WRITEBACK
	struct bdi_writeback	*i_wb;		/* the associated cgroup wb */

	/* foreign inode detection, see wbc_detach_inode() */
	int			i_wb_frn_winner;
	u16			i_wb_frn_avg_time;
	u16			i_wb_frn_history;
#endif
	struct list_head	i_lru;		/* inode LRU list */
	struct list_head	i_sb_list;
	struct list_head	i_wb_list;	/* backing dev writeback list */
	union {
		struct hlist_head	i_dentry;
		struct rcu_head		i_rcu;
	};
	atomic64_t		i_version;
	atomic64_t		i_sequence; /* see futex */
	atomic_t		i_count;
	atomic_t		i_dio_count;
	atomic_t		i_writecount;
#if defined(CONFIG_IMA) || defined(CONFIG_FILE_LOCKING)
	atomic_t		i_readcount; /* struct files open RO */
#endif
	union {
		const struct file_operations	*i_fop;	/* former ->i_op->default_file_ops */
		void (*free_inode)(struct inode *);
	};
	struct file_lock_context	*i_flctx;
	struct address_space	i_data;
	struct list_head	i_devices;
	union {
		struct pipe_inode_info	*i_pipe;
		struct cdev		*i_cdev;
		char			*i_link;
		unsigned		i_dir_seq;
	};

	__u32			i_generation;

#ifdef CONFIG_FSNOTIFY
	__u32			i_fsnotify_mask; /* all events this inode cares about */
	struct fsnotify_mark_connector __rcu	*i_fsnotify_marks;
#endif

#ifdef CONFIG_FS_ENCRYPTION
	struct fscrypt_info	*i_crypt_info;
#endif

#ifdef CONFIG_FS_VERITY
	struct fsverity_info	*i_verity_info;
#endif

	void			*i_private; /* fs or device private pointer */
} __randomize_layout;

```

## Miscellaneous Character Drivers

Instead of creating a full blown character driver everytime that we require one,
Linux provides the `miscellaneous character driver`. All the misc drivers
have the `10` major number. A specific minor number can be requested or the
kernel can generate one dynamically.

The main functions and data structures for the miscellaneous drivers are in
[include/linux/miscdevice.h].

```c
struct miscdevice  {
	int minor;
	const char *name;
	const struct file_operations *fops;
	struct list_head list;
	struct device *parent;
	struct device *this_device;
	const struct attribute_group **groups;
	const char *nodename;
	umode_t mode;
};

extern int misc_register(struct miscdevice *misc);
extern void misc_deregister(struct miscdevice *misc);
```

Commonly we only fill the following information in `miscdevice`:

```c
static struct miscdevice custom_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = CUSTOM_MISC_NAME,
	.fops = &custom_fops,

};
```

[include/linux/fs.h]: https://elixir.bootlin.com/linux/v5.17.4/source/include/linux/fs.h
[include/linux/miscdevice.h]: https://elixir.bootlin.com/linux/v5.17.4/source/include/linux/miscdevice.h

