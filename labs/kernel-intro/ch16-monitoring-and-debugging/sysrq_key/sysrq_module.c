/*
 * Extending SysRq keys.
 *
 * It is possible to extend the SysRq key mechanism to enable custom
 * defined keys and actions.  All the information needed can be found
 * in include/linux/sysrq.h.
 *
 * The relevant structures and functions are:
 *
 *  #include <linux/sysrq.h>
 *
 *  struct sysrq_key_op {
 *      void (*handler)(int, struct tty_struct *);
 *      char *help_msg;
 *      char *action_msg;
 *  };
 *  int register_sysrq_key(int, struct sysrq_key_op *);
 *  int unregister_sysrq_key(int, struct sysrq_key_op *);
 *
 * where the key value (say 'x') is supplied as the first argument to
 * the handler function.
 *
 * The purpose and usage of the other functions and variables should
 * be clear from the definition.
 *
 * Write a simple module that registers a new key, that can do as
 * little as print a message.  See what occurs when you get the help
 * message.
 *
 * Make sure that you have turned this option on in your kernel at
 * compilation and in /proc/sys/kernel/sysrq.  If you have keyboard
 * problems you may need to do something like
 *
 *      sudo sh -c 'echo x > /proc/sysrq-trigger'
 *
 * Make sure the sys req keys are enable:
 *
 *      cat /proc/sys/kernel/sysrq
 *      sudo sh -c 'echo 1 > /proc/sys/kernel/sysrq'
 *      cat /proc/sys/kernel/sysrq
 */


#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/sysrq.h>


#define CUSTOM_SYSRQ_KEY 'x'

static void custom_sysrq_handler(int key)
{
	pr_info("%s: Handler for %c\n", __func__, CUSTOM_SYSRQ_KEY);
}

static struct sysrq_key_op custom_sysrq_key_op = {
	.handler = custom_sysrq_handler,
	.help_msg = "Custom sysrq key `x`",
	.action_msg = "`x` sysrq key in action",
};

static int __init custom_sysrq_init(void)
{
	int ret;
	pr_info("%s: Loading the module %s at address %p\n",
			KBUILD_MODNAME, KBUILD_MODNAME, init_module);

	ret = register_sysrq_key(CUSTOM_SYSRQ_KEY, &custom_sysrq_key_op);
	if (ret == 0) {
		pr_info("%s: custom sysrq key was registered!\n", KBUILD_MODNAME);
		pr_info("%s: Type: sudo sh -c 'echo x > /proc/sysrq-trigger'\n",
				KBUILD_MODNAME);
	} else {
		pr_err("%s: Failed to register custom sysrq key!\n", KBUILD_MODNAME);
	}

	return ret;
}

static void __exit custom_sysrq_exit(void)
{
	int ret;
	pr_info("%s: Exiting the module %s at address %p\n",
			KBUILD_MODNAME, KBUILD_MODNAME, cleanup_module);

	ret = unregister_sysrq_key(CUSTOM_SYSRQ_KEY, &custom_sysrq_key_op);
	if (ret == 0) {
		pr_info("%s: custom sysrq key was unregistered!\n", KBUILD_MODNAME);
	} else {
		pr_err("%s: Failed to unregister custom sysrq key!\n", KBUILD_MODNAME);
	}
}

module_init(custom_sysrq_init);
module_exit(custom_sysrq_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Custom sysrq key module");
MODULE_LICENSE("GPL v2");


