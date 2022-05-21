// SPDX-License-Identifier: GPL-2.0
/*
 * Module that prints the physical and virtual memory addresses for a process.
 */

#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/pagewalk.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/types.h>

#define PHYS_MEM_DEV_NAME "physmem"
#define BUFFER_SIZE 30

static struct device *phys_mem_dev;

int walk_pgd(pgd_t *pgd, unsigned long addr, unsigned long next,
		struct mm_walk *walk)
{
	dev_info(phys_mem_dev, " %18llX (PGD)    %18lX", pgd_val(*pgd), addr);

	return 0;
}

int walk_pte(pte_t *pte, unsigned long addr, unsigned long next,
		struct mm_walk *walk)
{
	dev_info(phys_mem_dev, " %18llX (PTE)    %18lX", pte_val(*pte), addr);

	return 0;
}

static const struct mm_walk_ops phys_walk_ops = {
	.pgd_entry = walk_pgd,
	.pte_entry = walk_pte,
};

static inline void print_vma_areas(struct mm_struct *mm)
{
	struct vm_area_struct *vma;
	int j = 0;
	unsigned long start;
	unsigned long end;
	unsigned long length;
	unsigned long flags;

	dev_info(phys_mem_dev,
			" # vmas         vma(ptr)        start          end       length(Hex=Decimal=KB)     RWESH\n");

	vma = mm->mmap;
	while (vma) {
		j++;
		start = vma->vm_start;
		end = vma->vm_end;
		length = end - start;
		flags = vma->vm_flags;

		dev_info(phys_mem_dev,
			"%6d: %16p %12lX %12lX   %8lX =%8ld = %6ldKB  %s%s%s%s%s\n", j,
			vma, start, end, length, length, length / 1000,
			flags & VM_READ ? "R" : "-", flags & VM_WRITE ? "W" : "-",
			flags & VM_EXEC ? "E" : "-", flags & VM_SHARED ? "S" : "-",
			flags & VM_HUGEPAGE ? "H" : "-");

		vma = vma->vm_next;
	}
}

static inline void print_phys_mem(struct mm_struct *mm)
{
	struct vm_area_struct *vma;
	int j = 0;
	unsigned long start;
	unsigned long end;
	int ret;

	vma = mm->mmap;
	while (vma) {
		j++;
		start = vma->vm_start;
		end = vma->vm_end;

		dev_info(phys_mem_dev, " ");
		dev_info(phys_mem_dev, "         Physical Address       Virtual Address");

		// walk_page_range() symbol has to be exported in the Kernel.
		ret = walk_page_range(mm, start, end, &phys_walk_ops, NULL);
		if (ret) {
			dev_err(phys_mem_dev, "Failed to walk page range [%ld, %ld]\n",
					start, end);
		}

		vma = vma->vm_next;
	}
}

static void print_mem(struct task_struct *tsk)
{
	struct mm_struct *mm = tsk->mm;

	mmap_read_lock(mm);

	print_vma_areas(mm);
	print_phys_mem(mm);

	mmap_read_unlock(mm);
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
static ssize_t phys_mem_per_proc_write(struct file *file,
		const char __user *buf, size_t lbuf,
		loff_t *ppos)
{
	char kbuffer[BUFFER_SIZE];
	int nbytes;
	int ret;
	pid_t pid;
	struct task_struct *tsk;

	nbytes = simple_write_to_buffer(kbuffer, BUFFER_SIZE, ppos, buf, lbuf);

	kbuffer[lbuf > BUFFER_SIZE ? BUFFER_SIZE : lbuf] = '\0';
	ret = kstrtoint(kbuffer, 10, &pid);
	if (ret)
		return -EINVAL;

	tsk = pid_task(find_vpid(pid), PIDTYPE_PID);
	if (!tsk) {
		dev_info(phys_mem_dev, "Task with pid=%d not found", pid);
		goto out;
	}

	if (tsk->mm == NULL) {
		dev_info(phys_mem_dev, "Kernel threads don't have 'mm' set");
		goto out;
	}

	dev_info(phys_mem_dev, "Page information for PID [%d]", pid);
	print_mem(tsk);

out:
	return nbytes;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.write = phys_mem_per_proc_write,
};

static struct miscdevice phys_mem_per_proc_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = PHYS_MEM_DEV_NAME,
	.fops = &fops,
};

static int __init phys_mem_per_proc_init(void)
{
	if (misc_register(&phys_mem_per_proc_dev)) {
		dev_err(phys_mem_dev, "Failed to initialize module!\n");
		return -EBUSY;
	}

	phys_mem_dev = phys_mem_per_proc_dev.this_device;
	dev_info(phys_mem_dev, "init() succeedded in registering device %s\n",
		PHYS_MEM_DEV_NAME);

	return 0;
}

static void __exit phys_mem_per_proc_exit(void)
{
	dev_info(phys_mem_dev, "Unregistering device %s\n", PHYS_MEM_DEV_NAME);
	misc_deregister(&phys_mem_per_proc_dev);
}

module_init(phys_mem_per_proc_init);
module_exit(phys_mem_per_proc_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Prints the physical and vitual memory per process");
MODULE_LICENSE("GPL v2");
