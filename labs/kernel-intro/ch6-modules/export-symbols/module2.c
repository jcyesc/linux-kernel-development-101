// SPDX-License-Identifier: GPL-2.0
/*
 * Module that uses exported symbols.
 */

#include <linux/init.h>
#include <linux/module.h>


extern int module1_counter;
void module1_increment_counter(void);

// The __init macro indicates the kernel that the function should be removed
// from memory after initializing.
static int __init module2_init(void)
{
	pr_info("---- module2_init() ----");
	pr_info("module2 -> before module1_increment_counter = %d", module1_counter);
	module1_increment_counter();
	pr_info("module2 -> after module1_increment_counter = %d", module1_counter);

	return 0;
}

// The __exit macro does not do much except group all such labeled material
// together in the executable in an area not likely to be cached.
static void __exit module2_exit(void)
{
	pr_info("---- module2_exit() ----");
}

module_init(module2_init);
module_exit(module2_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Module that exports symbols");
MODULE_LICENSE("GPL v2");




