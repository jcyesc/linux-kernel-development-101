/*
 * Defines Generic file operations functions to be used by the drivers.
 */

#ifndef CHAPTER_13_MEM_USER_AND_KERNEL_SPACE
#define CHAPTER_13_MEM_USER_AND_KERNEL_SPACE

#include <linux/cdev.h>     // cdev utilities
#include <linux/errno.h>
#include <linux/fs.h>       // File operations.
// https://elixir.bootlin.com/linux/v5.15.12/source/include/linux/init.h
#include <linux/init.h>     // Macros for the module.
#include <linux/module.h>   // for modules
#include <linux/printk.h>   // printk, pr_info
#include <linux/slab.h>     // kmalloc, kfree
#include <linux/types.h>    // loff_t is defined here.


#define MEM_USER_KERNEL_SPACE "memuserkernel101"
#define OBJ_SIZE 2 * PAGE_SIZE
#define OBJ_BUFFER OBJ_SIZE - sizeof(size_t)

static struct kmem_cache *objs_cache;
static struct device *misc_dev;

/* The size of obj_buf info is OBJ_SIZE.
 *
 * @buffer Keeps the data written by user-space.
 * @nbytes_written Keeps the number of bytes written to the buffer.
 */
struct obj_buf {
	char buffer[OBJ_BUFFER];
	size_t nbytes;
};

_Static_assert(sizeof(struct obj_buf) == OBJ_SIZE,
	"size not equals to OBJ_SIZE");

/*
 * Creates a buffer that will be used for the read/write file_operations. It
 * will be freed after calling the release() function.
 *
 * For every call to the function open() in file_operations, a new buffer
 * will be created using the slab allocator.
 */
static int generic_cdev_open(struct inode *inode, struct file *file)
{
	dev_t dev_num = inode->i_rdev;
	struct obj_buf *obj;

	dev_info(misc_dev, "generic_cdev_open(): Opening device %s: MAJOR %d MINOR %d\n",
			MEM_USER_KERNEL_SPACE, MAJOR(dev_num), MINOR(dev_num));
	dev_info(misc_dev, "module_refcount=%d\n", (int)module_refcount(THIS_MODULE));

	obj = kmem_cache_zalloc(objs_cache, GFP_ATOMIC);
	if (!obj) {
		pr_err("Failed to create a cache object.\n");
		return -ENOMEM;
	}

	file->private_data = (void *)obj;

	return 0;
}

static int generic_cdev_release(struct inode *inode, struct file *file)
{
	struct obj_buf *obj = (struct obj_buf *)file->private_data;
	dev_info(misc_dev, "generic_cdev_release(): Releasing device: %s\n",
		MEM_USER_KERNEL_SPACE);
	dev_info(misc_dev, "generic_cdev_release(): MSG: %s\n", obj->buffer);

	kmem_cache_free(objs_cache, file->private_data);

	return 0;
}

/**
 * Read from the character device and copy to the user's buffer.
 *
 * @file The file object that was created by the open() function.
 * @buf  The user's buffer that will be populated with the content of obj_buf->buf.
 * @lbuf The length of the buffer.
 * @ppos The present position pointer.
 * @return The number of characters that were read.
 */
static ssize_t generic_cdev_read(
		struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
	int nbytes = 0;
	int bytes_to_read;
	int maxbytes;
	char *kernbuf;
	struct obj_buf *obj = (struct obj_buf *)file->private_data;

	dev_info(misc_dev, "generic_cdev_read(): Starts lbuf=%d, *ppos=%d, nbytes=%d\n",
		(int)lbuf, (int)*ppos, (int)obj->nbytes);

	maxbytes = obj->nbytes - *ppos;
	bytes_to_read = maxbytes > lbuf ? lbuf : maxbytes;

	if (bytes_to_read == 0)
		dev_info(misc_dev, "generic_cdev_read(): The %s reached the end of the file\n",
			MEM_USER_KERNEL_SPACE);

	kernbuf = obj->buffer + *ppos;
	// int put_user(src_expr, dest_ptr);
	while ((nbytes < bytes_to_read) && !put_user(*kernbuf, (buf + nbytes))) {
		nbytes++;
		kernbuf++;
	}

	*ppos += nbytes;

	dev_info(misc_dev, "generic_cdev_read(): Ends nbytes=%d, *ppos=%d\n, msg=%s",
		nbytes, (int)*ppos, obj->buffer);

	return nbytes;
}

/**
 * Writes to the character device.
 *
 * @file The file object that was created by the open() function.
 * @buf  The user's buffer to write to the character device.
 * @lbuf The length of the buffer.
 * @ppos The present position pointer.
 * @return The number of characters that were written.
 */
static ssize_t
generic_cdev_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos)
{
    int nbytes = 0;
    int bytes_to_write;
    int maxbytes;
	char *kernbuf;
    struct obj_buf *obj = (struct obj_buf *)file->private_data;

    maxbytes = OBJ_BUFFER - *ppos;
    bytes_to_write = maxbytes > lbuf ? lbuf : maxbytes;

    dev_info(misc_dev, "generic_cdev_write(): Starts lbuf=%d, *ppos=%d, bytes_to_write=%d\n",
    	(int)lbuf, (int)*ppos, bytes_to_write);

    if(bytes_to_write == 0) {
        dev_info(misc_dev, "generic_cdev_write(): The %s reached the end of the file\n",
        	MEM_USER_KERNEL_SPACE);
        return 0;
    }

    kernbuf = obj->buffer + *ppos;
    // int get_user(lvalue, src_ptr);
    while (nbytes < bytes_to_write && !get_user(*kernbuf, buf + nbytes)) {
        nbytes++;
        kernbuf++;
    }

    obj->nbytes = nbytes;
    *ppos += nbytes;
    dev_info(misc_dev, "generic_cdev_write(): Ends nbytes=%d, *ppos=%d\n",
        nbytes, (int)*ppos);

    return nbytes;
}

/*
 * When the offset in the obj_buf.buffer needs to be updated, llseek is used.
 *
 * Useful references:
 *   https://man7.org/linux/man-pages/man2/lseek.2.html
 *   https://elixir.bootlin.com/linux/latest/source/include/linux/fs.h#L1981
 *   https://elixir.bootlin.com/linux/latest/source/fs/read_write.c#L236
 */
loff_t generic_cdev_llseek(struct file *file, loff_t offset, int whence)
{
	loff_t retval;
	struct obj_buf *obj = (struct obj_buf *)file->private_data;

	switch (whence) {
		case SEEK_SET: /* seek relative to beginning of file */
			if (offset >= obj->nbytes || offset > OBJ_BUFFER) {
				retval = -EINVAL;
				goto out;
			}
			/*
			 * Update the file->f_pos directly. file->f_pos is the same pointer
			 * that is passed to read() and write(). Note, in the read() and
			 * write() methods, loff_t *ppos has to be used instead of
			 * file->f_pos. See pread() and pwrite() for more information.
			 */
			file->f_pos = offset;
			retval = offset;
			break;
		case SEEK_END:  /* seek relative to end of file */
		case SEEK_CUR:  /* seek relative to current file position */
		case SEEK_DATA: /* seek to the next data */
		default:
			retval = -EINVAL;
			break;
	}

	out:
		dev_info(misc_dev, "generic_cdev_llseek(): offset=%d\n", (int)retval);

	return retval;
}



#endif // CHAPTER_13_MEM_USER_AND_KERNEL_SPACE
