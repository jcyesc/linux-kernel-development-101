/**
 * spin-lock module
 *
 * An spinlock is a kernel construction that is used to protect critical
 * regions. It will spin while waiting for a resource to be available and it
 * doesn't sleep while waiting.
 *
 * This module shows how to use spin-locks. In order to do that, it creates
 * and starts 2 kthreads that get the spinlock before printing the kthread
 * name.
 *
 * To see the output, execute:
 *
 * $ tail -f /var/log/messages
 */

#include <linux/atomic.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/version.h>

spinlock_t print_lock;


static int kthread_handler(void *data)
{
	int i;
	char *thread_name = data;
	unsigned long msecs = 1000;

	for (i = 1; i <= 20; i++) {
		spin_lock(&print_lock);
		pr_info("Thread %s has the spinlock for %d time\n", thread_name, i);
		spin_unlock(&print_lock);
		// The kthread can NOT sleep while having the spin_lock.
		msleep_interruptible(msecs);
	}

	return 0;
}


static int __init spin_lock_module_init(void)
{
	int ret;
	struct task_struct *task;

	pr_info("Trying to load module %s\n", KBUILD_MODNAME);
	spin_lock_init(&print_lock);

	// Create and start a kthread.
	task = kthread_run(kthread_handler, "THREAD_1 HANDLER", "THREAD_1");
	if (task == ERR_PTR(-ENOMEM)) {
		ret = -ENOMEM;
		goto out;
	}

	// Create and start a kthread.
	task = kthread_run(kthread_handler, "THREAD_2 HANDLER", "THREAD_2");
	if (task == ERR_PTR(-ENOMEM)) {
		ret = -ENOMEM;
		goto out;
	}

	return 0;

	out:
		pr_err("There was an error in %s\n", KBUILD_MODNAME);
		return ret;
}

static void __exit spin_lock_module_exit(void)
{
	pr_info("Exiting from module %s\n", KBUILD_MODNAME);
}

module_init(spin_lock_module_init);
module_exit(spin_lock_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("John");
MODULE_DESCRIPTION("Usage of spin-locks");


