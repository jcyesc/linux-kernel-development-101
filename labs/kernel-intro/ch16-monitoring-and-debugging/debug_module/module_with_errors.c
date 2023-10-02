// SPDX-License-Identifier: GPL-2.0
/*
 * Module with errors
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": "  __FILE__ ": " fmt

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>

static int __init module_with_errors_init(void)
{
	int *ptr;

	ptr = (int *) 0xBEEF;

	pr_info("%d : *ptr = %d", __LINE__, *ptr);

	return 0;
}

static void __exit module_with_errors_exit(void)
{
	pr_info("Exiting module with errors");
}


module_init(module_with_errors_init);
module_exit(module_with_errors_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Module that contains pointer errors");

