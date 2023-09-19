// SPDX-License-Identifier: GPL-2.0
// Hello World Module.
//
// This module dumps the stack. Example of the output is below:
//
// [   69.009863] hello_world_mod: hello_init() Init Hello World!
// [   69.010149] hello_world_mod: hello_init() Dumping the stack
// [   69.010377] CPU: 0 PID: 139 Comm: insmod Tainted: G           O       6.3.13 #2
// [   69.011062] Hardware name: linux,dummy-virt (DT)
// [   69.011437] Call trace:
// [   69.011744]  dump_backtrace+0x9c/0xf8
// [   69.012067]  show_stack+0x18/0x24
// [   69.012209]  dump_stack_lvl+0x48/0x60
// [   69.012345]  dump_stack+0x18/0x24
// [   69.012626]  hello_init+0x40/0x1000 [hello_world_mod]
// [   69.013366]  do_one_initcall+0x70/0x1b4
// [   69.013482]  do_init_module+0x48/0x1c8
// [   69.013840]  load_module+0x1ab0/0x1bac
// [   69.013994]  __do_sys_finit_module+0xc0/0x114
// [   69.014236]  __arm64_sys_finit_module+0x20/0x2c
// [   69.014419]  invoke_syscall+0x44/0x104
// [   69.014593]  el0_svc_common.constprop.0+0x44/0xec
// [   69.014789]  do_el0_svc+0x38/0x98
// [   69.014928]  el0_svc+0x2c/0x84
// [   69.015089]  el0t_64_sync_handler+0xb8/0xbc
// [   69.015306]  el0t_64_sync+0x190/0x194


#define pr_fmt(fmt) KBUILD_MODNAME ": %s() " fmt, __func__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>


static int __init hello_init(void)
{
	pr_info("Init Hello World!\n");

	pr_info("Dumping the stack");
	dump_stack();

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

