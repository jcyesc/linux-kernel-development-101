// SPDX-License-Identifier: GPL-2.0
/*
 * Module that shows how to use the xarray(s) structure.
 *
 * - Declare an xarray
 * - Init an xarray
 * - Store elements in the xarray
 * - Find data by id in the xarray
 * - Free xarray
 *
 * Note: idr is the map implementation in the linux kernel that
 *       only supports numbers.
 *
 * More information in:
 *
 * https://elixir.bootlin.com/linux/v7.1.3/source/include/linux/xarray.h
 * https://www.kernel.org/doc/html/v7.1/core-api/xarray.html
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/xarray.h>

#define NUM_USERS 5

static DEFINE_XARRAY(user_xarray);

struct user {
	int id;
	char name[20];
};

static struct user users[NUM_USERS] = {
		{.id = 0, .name = "root:0"},
        {.id = 1, .name = "silver:1"},
		{.id = 2, .name = "diana:2"},
        {.id = 3, .name = "mega:3"},
		{.id = 4, .name = "user:4"},
};

static int __init xarray_mod_init(void)
{
	struct user *user;
	unsigned long index;
	int ret;

	pr_info("Basic example of xarray\n");

	for (int i = 0; i < NUM_USERS; i++) {
		ret = xa_insert(&user_xarray, i, &users[i], GFP_KERNEL);
		if (ret)
			pr_err("Failed to insert in xarray");
	}

	for (int i = NUM_USERS - 1; i >= 0; i--) {
		user = xa_load(&user_xarray, i);
		if (user)
			pr_info("xa_load id: %d, name: %s\n", user->id, user->name);
	}

	// Unknown index.
	user = xa_load(&user_xarray, 45);
	if (!user)
		pr_info("xa_load() didn't find the id: %d", 45);

	pr_info("Using xa_for_each() to iterate:\n");
	// Note that index hasn't been initialized.
	xa_for_each(&user_xarray, index, user) {
		pr_info("xa_load id: %d, name: %s\n", user->id, user->name);
	}

	user = xa_erase(&user_xarray, 3);
	if (user)
		pr_info("Erased user with id: %d and name: %s)\n", user->id, user->name);

	pr_info("Trying to erase an entry that does not exist");
	user = xa_erase(&user_xarray, 2253);
	if (!user)
		pr_info("Entry was NOT erased!");

	for (int i = NUM_USERS - 1; i >= 0; i--) {
		user = xa_load(&user_xarray, i);
		if (user)
			pr_info("xa_load id: %d, name: %s\n", user->id, user->name);
	}

	return 0;
}

static void __exit xarray_mod_exit(void)
{
	struct user *user;
	unsigned long index;

	xa_for_each(&user_xarray, index, user) {
		pr_info("Destroying index: %lu, id: %d, name: %s\n", index, user->id,
		    user->name);
		xa_erase(&user_xarray, index);
		// kfree(entry);
	}

	xa_destroy(&user_xarray);
}

module_init(xarray_mod_init);
module_exit(xarray_mod_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("xarray basic example");
MODULE_LICENSE("GPL");
