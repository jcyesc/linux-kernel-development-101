/*
 * Module that uses a {@code thunder_cat} reference and increment
 * and decrement its {@code kref} counter.
 */

#include <linux/init.h>
#include <linux/kref.h>
#include <linux/module.h>
#include <linux/printk.h>

#include "thunder_cat.h"

// Extern symbols defined in krefs_module.c
extern struct thunder_cat *panthro;
extern void (*release_thunder_cat)(struct kref *ref);

static int __init krefs_client_module1_init(void)
{
	pr_info("%s: Loading the module %s\n", KBUILD_MODNAME, KBUILD_MODNAME);

	// Incrememting the kref counter.
	kref_get(&panthro->refcount);
	thunder_cat_print(panthro);

	return 0;
}

static void __exit krefs_client_module1_exit(void)
{
	pr_info("%s: Exiting the module %s\n", KBUILD_MODNAME, KBUILD_MODNAME);
	kref_put(&panthro->refcount, release_thunder_cat);
}

module_init(krefs_client_module1_init);
module_exit(krefs_client_module1_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("John");
MODULE_DESCRIPTION("krefs client 1 for thunder_cat ref");
