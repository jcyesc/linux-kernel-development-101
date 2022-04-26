/* **************** LFD420:5.14 s_20/lab_maxmem.c **************** */
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
 * Testing Maximum Memory Allocation
 *
 * See how much memory you can obtain dynamically, using both
 * kmalloc() and __get_free_pages().
 *
 * Start with requesting 1 page of memory, and then keep doubling
 * until *your request fails for each type fails.
 *
 * Make sure you free any memory you receive.
 *
 * You'll probably want to use GFP_ATOMIC rather than
 * GFP_KERNEL. (Why?)

 * If you have trouble getting enough memory due to memory
 * fragmentation trying writing a poor-man's defragmenter, and then
 * running again.  The defragmenter can just be an application that
 * grabs all available memory, uses it, and then releases it when
 * done, thereby clearing the caches.  You can also try the command
 *
 * sync; echo 3 > /proc/sys/vm/drop_caches .
 *
 * Try the same thing with vmalloc().  Rather than
 * doubling allocations, start at 4 MB and increase in 4 MB incremements
 * until failure results.  Note this may hang while loading. (Why?)
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

#define VMALLOC_INCREMENT  (1024 * 1024 * 4) // 4MB

static int __init my_init(void)
{
	static char *kbuf;
	static unsigned long order;
	int size;

	/* try __get_free_pages__ */
	pr_info("Allocating memory with __get_free_pages\n");
	for (size = PAGE_SIZE, order = 0; order < MAX_ORDER + 1;
	     order++, size *= 2) {
		pr_info(" order=%2ld, pages=%5ld, size=%8d ", order,
			size / PAGE_SIZE, size);
		kbuf = (char *)__get_free_pages(GFP_ATOMIC, order);
		if (!kbuf) {
			pr_err("... __get_free_pages failed\n");
			break;
		}
		pr_info("... __get_free_pages OK\n");
		free_pages((unsigned long)kbuf, order);
	}

	/* try kmalloc */
	pr_info("Allocating memory with kmalloc\n");
	for (size = PAGE_SIZE, order = 0; order < MAX_ORDER + 1;
	     order++, size *= 2) {
		pr_info(" order=%2ld, pages=%5ld, size=%8d ", order,
			size / PAGE_SIZE, size);
		kbuf = kmalloc((size_t)size, GFP_ATOMIC);
		if (!kbuf) {
			pr_err("... kmalloc failed\n");
			break;
		}
		pr_info("... kmalloc OK\n");
		kfree(kbuf);
	}

	/* try vmalloc */
	pr_info("Allocating memory with vmalloc\n");
	for (size = VMALLOC_INCREMENT; 1; size += VMALLOC_INCREMENT) {
		pr_info(" order=?, pages=%5ld, size=%8d ", size / PAGE_SIZE, size);
		kbuf = vmalloc((size_t)size);
		if (!kbuf) {
			pr_err("... vmalloc failed\n");
			break;
		}
		pr_info("... vmalloc OK\n");
		vfree(kbuf);
	}

	return 0;
}

static void __exit my_exit(void)
{
	pr_info("Module Unloading\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD420:5.14 s_20/lab_maxmem.c");
MODULE_LICENSE("GPL v2");
