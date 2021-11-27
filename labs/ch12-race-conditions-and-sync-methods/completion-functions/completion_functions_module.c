/*
 * Completion Functions
 *
 * The completion functions allow you to wait for certain events. Completion
 * functions are optimized for high contention.
 *
 * This module will define and export a {@code completion} struct and it will
 * wait till an array is populated by the producer_module.
 *
 * Note: For this example, instead of having 2 modules, we could have defined
 * 2 kthreads, one that waits for the completion, and another that performs
 * the task that the completion function is waiting.
 */


#include <linux/completion.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/sched.h>

#define SHARED_MSG_SIZE 256


/**
 * Completion function that will wait for the producer to build a message.
 */
DECLARE_COMPLETION(build_message);
EXPORT_SYMBOL(build_message);

int shared_msg_size = SHARED_MSG_SIZE;
EXPORT_SYMBOL(shared_msg_size);

char shared_msg[SHARED_MSG_SIZE];
EXPORT_SYMBOL(shared_msg);


/*
 * Handler that waits till the completion function is done.
 */
static int completion_kthread_handler(void *data)
{
	pr_info("cfm-kthread: Waiting for completion function");
	wait_for_completion(&build_message);

	pr_info("cfm-kthread: Completion function is DONE.\n");
	pr_info("cfm-kthread: The shared message is: %s", shared_msg);

	return 0;
}

static int __init completion_functions_module_init(void)
{
	int ret;
	struct task_struct *task;

	pr_info("cfm: Loading the module %s\n", KBUILD_MODNAME);

	// Create and start a kthread.
	task = kthread_run(completion_kthread_handler,
			"Completion Handler", "Completion Handler");
	if (task == ERR_PTR(-ENOMEM)) {
		ret = -ENOMEM;
		goto out;
	}

	return 0;

	out:
		pr_err("There was an error in %s\n", KBUILD_MODNAME);

	return ret;
}

static void __exit completion_functions_module_exit(void)
{
	pr_info("cfm: Exiting from module %s\n", KBUILD_MODNAME);
}

module_init(completion_functions_module_init);
module_exit(completion_functions_module_exit);

MODULE_AUTHOR("John");
MODULE_DESCRIPTION("LFD420: Use of completion functions.");
MODULE_LICENSE("GPL v2");

