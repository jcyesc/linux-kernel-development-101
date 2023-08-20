// SPDX-License-Identifier: GPL-2.0
// Hello World Module.

#define pr_fmt(fmt) KBUILD_MODNAME ": %s() " fmt, __func__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>


static int __init hello_init(void)
{
	pr_info("Init Hello World!\n");

	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("Exit Hello World!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Hello World module");
MODULE_LICENSE("GPL v2");

