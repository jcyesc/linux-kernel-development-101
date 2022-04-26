/* **************** LFD420:5.14 s_21/lab_vma.c **************** */
/*
 * The code herein is: Copyright the Linux Foundation, 2021
 *
 * This Copyright is retained for the purpose of protecting free
 * redistribution of source.
 *
 *     URL:    https://training.linuxfoundation.org
 *     email:  info@linuxfoundation.org
 *
 * This code is distributed under Version 2 of the GNU General Public
 * License, which you should have received with the source.
 *
 */
/*
 * Examining Virtual Memory Areas
 *
 * Write a simple module which examines the address space for a
 * process.
 *
 * For each virtual memory area it finds, print out the address of the
 * vm_area_struct describing it, and the start, end, and length of the
 * address region.
 *
 * For simplicity you can use the current process (which will be
 * insmod during loading), or you can pass a process ID to the module.
 *
 * Examining the results can you identify some of the regions?  Look
 * at the maps entry in /proc for any process (such as /proc/self).
 *
 *
 *                         Address Space
 *                         mm_struct
 *                      /       |       \
 *                 vm_area   vm_area   vm_area
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/version.h>

static int pid;
module_param(pid, int, 0444);

static void printit(struct task_struct *tsk)
{
	struct mm_struct *mm;
	struct vm_area_struct *vma;
	int j = 0;
	unsigned long start;
	unsigned long end;
	unsigned long length;

	mm = tsk->mm;
	pr_info("mm = %p\n", mm);
	vma = mm->mmap;

	/* protect from simultaneous modification */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,8,0)
	mmap_read_lock(current->mm);
#else	
	down_read(&mm->mmap_sem);
#endif	
	pr_info
	    ("vmas:                vma        start          end     length(Hex=Decimal)\n");

	while (vma) {
		j++;
		start = vma->vm_start;
		end = vma->vm_end;
		length = end - start;
		pr_info("%6d: %16p %12lx %12lx   %8lx=%8ld\n",
			j, vma, start, end, length, length);
		vma = vma->vm_next;
	}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,8,0)
	mmap_read_unlock(current->mm);
#else	
	up_read(&mm->mmap_sem);
#endif
}

static int __init my_init(void)
{
	struct task_struct *tsk;

	if (pid == 0) {
		tsk = current;
		pid = current->pid;
	} else {
		/*        tsk = find_task_by_vpid (pid); */
		tsk = pid_task(find_vpid(pid), PIDTYPE_PID);
	}
	if (!tsk)
		return -1;
	pr_info("Examining vma's for pid=%d, command=%s\n", pid, tsk->comm);
	printit(tsk);
	return 0;
}

static void __exit my_exit(void)
{
	pr_info("Module Unloading\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD420:5.14 s_21/lab_vma.c");
MODULE_LICENSE("GPL v2");

