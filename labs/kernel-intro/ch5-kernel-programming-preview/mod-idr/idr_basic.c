// SPDX-License-Identifier: GPL-2.0
/*
 * Module that shows how to use the idr (map) data structure.
 *
 * - Declare an idr
 * - Init an idr
 * - Allocate an id and associate a value
 * - Find data by id
 * - Free idr
 *
 * Note: idr is the map implementation in the linux kernel that only supports
 *       numbers.
 *
 * More information in:
 *
 * https://elixir.bootlin.com/linux/v5.18.1/source/include/linux/idr.h#L19
 * https://www.kernel.org/doc/html/latest/core-api/idr.html
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": %s() " fmt, __func__

#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/idr.h>
#include <linux/module.h>
#include <linux/printk.h>

#define NUM_USERS 5

struct idr usermap;

struct user {
	int id;
	char name[20];
};

static struct user users[NUM_USERS] = {
	{ .id = 0, .name = "root" },
	{ .id = 0, .name = "silver" },
	{ .id = 0, .name = "diana" },
	{ .id = 0, .name = "mega" },
	{ .id = 0, .name = "user" },
};

static int __init idr_mod_init(void)
{
	int i;
	int start = 23;
	int end = 100;
	int ret;
	int searchid = 23;
	struct user *usr;

	pr_info("Initializing module");

	idr_init(&usermap);

	// Populating the map.
	for (i = 0; i < NUM_USERS; i++) {
		ret = idr_alloc(&usermap, &users[i], start, end, GFP_KERNEL);
		if (ret == -ENOMEM || ret == -ENOSPC)
			return ret;

		users[i].id = ret;
		pr_info("user inserted id=%d name=%s", users[i].id, users[i].name);
	}

	usr = (struct user *)idr_find(&usermap, searchid);
	if (usr == NULL)
		pr_info("user with id=%d was not found!", searchid);
	else
		pr_info("user with id=%d is %s", usr->id, usr->name);

	return 0;
}

static void __exit idr_mod_exit(void)
{
	int i;

	pr_info("Exiting module");

	// Freeing the map.
	for (i = 0; i < NUM_USERS; i++)
		idr_remove(&usermap, users[i].id);

	if (idr_is_empty(&usermap))
		pr_info("User map is empty.");

	idr_destroy(&usermap);
}

module_init(idr_mod_init);
module_exit(idr_mod_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("idr library usage");
MODULE_LICENSE("GPL v2");

