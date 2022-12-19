#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>

/*
 * When the module is loaded, it will generate an oops message because a NULL
 * pointer is trying to be deferenced.
 */
static int __init oops_module_init(void)
{
	int *ptr = NULL;

	pr_info("%s: Loading the module %s at address %p\n",
			KBUILD_MODNAME, KBUILD_MODNAME, init_module);
	pr_info("*ptr = %d\n", *ptr);

	return 0;
}

static void __exit oops_module_exit(void)
{
	pr_info("%s: Exiting the module %s at address %p\n",
			KBUILD_MODNAME, KBUILD_MODNAME, cleanup_module);
}


module_init(oops_module_init);
module_exit(oops_module_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("oops message example");
MODULE_LICENSE("GPL v2";)
