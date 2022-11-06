// SPDX-License-Identifier: GPL-2.0

#define pr_fmt(fmt) KBUILD_MODNAME ": %s() " fmt, __func__

#include <linux/init.h>
#include <linux/module.h>


// The __init macro indicates the kernel that the function should be removed
// from memory after initializing.
static int __init my_init(void)
{
    pr_info("Hello: module loaded at 0x%p\n", my_init);
    return 0;
}

// The __exit macro does not do much except group all such labeled material
// together in the executable in an area not likely to be cached.
static void __exit my_exit(void)
{
    pr_info("Bye: module unloaded from 0x%p\n", my_exit);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Basic module");
MODULE_LICENSE("GPL v2");

