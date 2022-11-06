// SPDX-License-Identifier: GPL-2.0
/*
 * Module that exports a variable and a function.
 *
 * Note: After loading the module, check that the symbols appear in:
 *
 * $ sudo cat /proc/kallsyms
 */

#include <linux/init.h>
#include <linux/module.h>


static int module1_counter;

static void module1_increment_counter(void) {
	pr_info("module1_increment_counter() is incrementing module1_counter");
	module1_counter++;
}

// The __init macro indicates the kernel that the function should be removed
// from memory after initializing.
static int __init module1_init(void)
{
	pr_info("---- module1_init(void) ----");
	module1_counter = 0;

	return 0;
}

// The __exit macro does not do much except group all such labeled material
// together in the executable in an area not likely to be cached.
static void __exit module1_exit(void)
{
	pr_info("---- module1_exit(void) ----");
}

module_init(module1_init);
module_exit(module1_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Module that exports symbols");
MODULE_LICENSE("GPL v2");

EXPORT_SYMBOL(module1_counter);
EXPORT_SYMBOL(module1_increment_counter);
