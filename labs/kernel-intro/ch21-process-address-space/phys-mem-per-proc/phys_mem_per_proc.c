// SPDX-License-Identifier: GPL-2.0
/*
 * Module that prints the physical and virtual memory addresses for a process.
 */

#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/pagewalk.h>
#include <linux/printk.h>
#include <linux/rwlock.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/types.h>

#define PHYS_MEM_DEV_NAME "physmem"
#define BUFFER_SIZE 30

static int phys_dev_major = 500;
static int phys_dev_minor;
static struct cdev *phys_cdev;
static dev_t phys_cdev_num;
static const unsigned int num_cdevs = 1;

int walk_pgd(pgd_t *pgd, unsigned long addr, unsigned long next,
		struct mm_walk *walk)
{
	pr_info(" %18llX (PGD)    %18lX", pgd_val(*pgd), addr);

	return 0;
}

int walk_pte(pte_t *pte, unsigned long addr, unsigned long next,
		struct mm_walk *walk)
{
	pr_info(" %18llX (PTE)    %18lX", pte_val(*pte), addr);

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

	pr_info(" # vmas         vma(ptr)        start          end       length(Hex=Decimal=KB)     RWESH\n");

	vma = mm->mmap;
	while (vma) {
		j++;
		start = vma->vm_start;
		end = vma->vm_end;
		length = end - start;
		flags = vma->vm_flags;

		pr_info(
			"%6d: %16p %12lX %12lX   %8lX =%8ld = %6ldKB  %s%s%s%s%s\n", j,
			vma, start, end, length, length, length / 1000,
			flags & VM_READ ? "R" : "-",
			flags & VM_WRITE ? "W" : "-",
			flags & VM_EXEC ? "E" : "-",
			flags & VM_SHARED ? "S" : "-",
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

		pr_info(" ");
		pr_info("         Physical Address       Virtual Address");

		// walk_page_range() symbol has to be exported in the Kernel.
		ret = walk_page_range(mm, start, end, &phys_walk_ops, NULL);
		if (ret) {
			pr_err("Failed to walk page range [%ld, %ld]\n",
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
	char buffer[BUFFER_SIZE];
	int nbytes;
	int ret;
	int index;
	pid_t pid;
	struct task_struct *tsk;

	nbytes = simple_write_to_buffer(buffer, BUFFER_SIZE, ppos, buf, lbuf);

	index = lbuf > BUFFER_SIZE ? BUFFER_SIZE : lbuf;
	buffer[index - 1] = '\0';

	ret = kstrtoint(buffer, 10, &pid);
	if (ret)
		return -EINVAL;

//	When the a task_struct it is used, the lasklist_lock should be used. The
//	tasklist_lock hasn't been exported in the Kernel because it is not recommended
//	to use it in modules. Below it is how it is supposed to be used outside modules.
//	read_lock(&tasklist_lock);
	tsk = pid_task(find_vpid(pid), PIDTYPE_PID);
//	if (tsk)
//		get_task_struct(tsk);
//	read_unlock(&tasklist_lock);

	if (!tsk) {
		pr_info("Task with pid=%d not found", pid);
		goto out;
	}

	if (tsk->mm == NULL) {
		pr_info("Kernel threads don't have 'mm' set");
		goto out;
	}

	pr_info("Page information for PID [%d]", pid);
	print_mem(tsk);

out:
	return nbytes;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.write = phys_mem_per_proc_write,
};

static int __init phys_mem_per_proc_init(void)
{
	int ret;

	phys_cdev_num = MKDEV(phys_dev_major, phys_dev_minor);

	ret = register_chrdev_region(phys_cdev_num, num_cdevs, PHYS_MEM_DEV_NAME);
	if (ret < 0) {
		pr_err("Failed to register for %s\n", PHYS_MEM_DEV_NAME);
		return ret;
	}

	phys_cdev = cdev_alloc();
	if (!phys_cdev) {
		pr_err("Failed to allocate %s\n", PHYS_MEM_DEV_NAME);
		unregister_chrdev_region(phys_cdev_num, num_cdevs);
		return -EINVAL;
	}

	cdev_init(phys_cdev, &fops);

	ret = cdev_add(phys_cdev, phys_cdev_num, num_cdevs);
	if (ret < 0) {
		pr_err("Failed to add %s\n", PHYS_MEM_DEV_NAME);
		cdev_del(phys_cdev);
		unregister_chrdev_region(phys_cdev_num, num_cdevs);
		return ret;
	}

	pr_info("%s was registered successfully!\n", PHYS_MEM_DEV_NAME);

	return 0;
}

static void __exit phys_mem_per_proc_exit(void)
{
	cdev_del(phys_cdev);
	unregister_chrdev_region(phys_cdev_num, num_cdevs);

	pr_info("%s was unregistered successfully\n", PHYS_MEM_DEV_NAME);
}

module_init(phys_mem_per_proc_init);
module_exit(phys_mem_per_proc_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Prints the physical and vitual memory per process");
MODULE_LICENSE("GPL v2");
