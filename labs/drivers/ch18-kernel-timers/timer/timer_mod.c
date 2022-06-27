// SPDX-License-Identifier: GPL-2.0
/*
 * Module that shows how to use timers.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/timer.h>

static void print_msg(struct timer_list *timer_list)
{
	pr_info("MJ wore the #23 during most of his NBA career");
	pr_info("MJ is considered the GOAT");
}

DEFINE_TIMER(custom_timer, print_msg);

static int __init timer_mod_init(void)
{
	int seconds;

	pr_info("Init timer_mod");

	seconds = (HZ * 10); /* 10 seconds. */
	custom_timer.expires = jiffies + seconds;

	add_timer(&custom_timer);

	return 0;
}

static void __exit timer_mod_exit(void)
{
	pr_info("exit timer_mod");

	del_timer_sync(&custom_timer);
}

module_init(timer_mod_init);
module_exit(timer_mod_exit)

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Shows how to use timers");
MODULE_LICENSE("GPL v2");
