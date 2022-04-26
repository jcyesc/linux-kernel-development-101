/**
 * seq-lock module
 *
 * The seqlocks is a kernel construction that uses spinlocks underneath. The
 * seqlocks are used when:
 *
 * - There are more readers than writers.
 *
 * The seqlocks have these restrictions:
 *
 * - The readers never get the lock, so no side effects must be produced by
 *   altering the data.
 *
 * To see the output, execute:
 *
 * $ tail -f /var/log/messages
 */

#include <linux/delay.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/seqlock.h>
#include <linux/string.h>
#include <linux/types.h>


#define MSG_SIZE 256

DEFINE_SEQLOCK(my_seqlock);

char shared_msg[MSG_SIZE];

static void wait_n_second(int n) {
	uint64_t future;

	// Wait for n seconds
	future = jiffies + (HZ * n);
	while (jiffies < future);
}

/**
 * Read kthread handler that reads the {@code shared_msg} and
 * print it. It uses a seqlock to access {@code shared_msg}.
 */
static int read_kthread_handler(void *data)
{
	char *thread_name = data;
	int i, j;
	int nreads = 100;
	char copy_msg[MSG_SIZE];

	// Times to read the shared_msg nreads.
	for(i = 1; i <= nreads; i++) {
		unsigned int seq;
		j = 1;
		do {
			seq = read_seqbegin(&my_seqlock);
			memcpy(copy_msg, shared_msg, MSG_SIZE);
			copy_msg[MSG_SIZE - 1] = '\0';
			pr_info("%s is trying to read for %d time in %d tries",
					thread_name, i, j++);
		} while (read_seqretry(&my_seqlock, seq));

		pr_info("---- %s READ \"%s\" for %d time ----",
				thread_name, copy_msg, i);

		// When using spinlocks or seqlocks, we can't sleep in the critical
		// region.
		wait_n_second(1);
	}

	return 0;
}

/**
 * Write kthread handler that writes to {@code shared_msg} and print it.
 * It uses a seqlock to access {@code shared_msg}.
 */
static int write_kthread_handler(void *data)
{
	char *thread_name = data;
	int i;
	int nwrites = 9;
	char *msg = "@ message: ";
	const ssize_t size = strlen(msg);

	for (i = 1; i <= nwrites; i++) {
		write_seqlock(&my_seqlock);

		// Note: assumes that we won't write more than MSG_SIZE.
		// In production systems the size has to be validated.
		memcpy(shared_msg, msg, size);
		shared_msg[size] = '0' + i;
		shared_msg[size + 1] = '\0';
		shared_msg[MSG_SIZE - 1] = '\0';

		pr_info("######  %s wrote \"%s\" ######", thread_name, shared_msg);

		// When using spinlocks or seqlocks, we can't sleep in the critical
		// region.
		wait_n_second(2);

		write_sequnlock(&my_seqlock);
	}

	return 0;
}

static int __init seq_lock_module_init(void)
{
	int ret;
	struct task_struct *task;

	pr_info("Trying to load module %s\n", KBUILD_MODNAME);

	memcpy(shared_msg, "INITIAL SHARED MESSAGE", MSG_SIZE);

	// Create and start a READ kthread.
	task = kthread_run(read_kthread_handler, "READ_THREAD_1", "READ_THREAD_1");
	if (task == ERR_PTR(-ENOMEM)) {
		ret = -ENOMEM;
		goto out;
	}

	// Create and start a READ kthread.
	task = kthread_run(read_kthread_handler, "READ_THREAD_2", "READ_THREAD_2");
	if (task == ERR_PTR(-ENOMEM)) {
		ret = -ENOMEM;
		goto out;
	}

	// Create and start a WRITE kthread.
	task = kthread_run(write_kthread_handler, "WRITE_THREAD_1", "WRITE_THREAD_1");
	if (task == ERR_PTR(-ENOMEM)) {
		ret = -ENOMEM;
		goto out;
	}

	return 0;

	out:
		pr_err("There was an error in %s\n", KBUILD_MODNAME);
		return ret;
}

static void __exit seq_lock_module_exit(void)
{
	pr_info("Exiting from module %s\n", KBUILD_MODNAME);
}

module_init(seq_lock_module_init);
module_exit(seq_lock_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("John");
MODULE_DESCRIPTION("Usage of seq locks");

