// SPDX-License-Identifier: GPL-2.0
/*
 * Module to print the same system information that uname -a
 *
 * You'll have to examine and print out the fields in a structure of
 * type new_utsname, as defined in
 *
 * https://elixir.bootlin.com/linux/v5.16.5/source/include/uapi/linux/utsname.h#L25
 * https://elixir.bootlin.com/linux/v5.16.5/source/include/linux/utsname.h#L23
 *
 * This can be accessed through the name element in the exported structure
 * init_uts_ns, or through the inline function init_utsname() defined in
 * include/linux/utsname.h.
 *
 * Compare your results with the results of uname -a.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": %s() " fmt, __func__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/utsname.h>
#include <uapi/linux/utsname.h>


// The __init macro indicates the kernel that the function should be removed
// from memory after initializing.
static int __init system_info_init(void)
{
	struct new_utsname *utsname = init_utsname();

	pr_info("----- syste_info_init() -----");
	pr_info("sysname = %s \n"
			"nodename = %s \n"
			"release = %s \n"
			"version = %s \n"
			"machine = %s \n"
			"domainname = %s \n",
			utsname->sysname, utsname->nodename, utsname->release,
			utsname->version, utsname->machine, utsname->domainname);

	return 0;
}

// The __exit macro does not do much except group all such labeled material
// together in the executable in an area not likely to be cached.
static void __exit system_info_exit(void)
{
	pr_info("Exit from system_info module");
}

module_init(system_info_init);
module_exit(system_info_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("System info module");
MODULE_LICENSE("GPL v2");

