/*
 * Producer module
 *
 * This module populates the {@code shared_msg} array that is defined in
 * completion_functions_module.c. To notify that the array has been populated,
 * it uses the {@code build_message) completion function.
 */

#include <linux/completion.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/string.h>


/*
 * Variables define in the completation_functions_module.c.
 */
extern struct completion build_message;
extern int shared_msg_size;
extern char shared_msg[];

static int __init producer_module_init(void)
{
	char *msg = "Thundercats are alive!!";
	int msg_length = strlen(msg);


	if (shared_msg_size < msg_length) {
		pr_err("pm: The shared_msg_size is to small");
		return -1;
	}

	pr_info("pm: Loading the module %s\n", KBUILD_MODNAME);
	pr_info("pm: Producer building the message!\n");

	memcpy(shared_msg, msg, msg_length);
	shared_msg[msg_length] = '\0';

	// Notify that the tasks has been finished.
	// Note: The module has to be loaded completely, otherwise the exported
	// symbols won't be available.
	complete(&build_message);
	pr_info("pm: Task completed\n");

	return 0;
}

static void __exit producer_module_exit(void)
{
	pr_info("Exiting from module %s\n", KBUILD_MODNAME);
}

module_init(producer_module_init);
module_exit(producer_module_exit);

MODULE_AUTHOR("John");
MODULE_DESCRIPTION("LFD420: Producer module that uses a completion struct.");
MODULE_LICENSE("GPL v2");

