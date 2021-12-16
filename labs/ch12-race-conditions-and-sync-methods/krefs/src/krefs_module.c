/*
 * Krefs module
 *
 * Krefs allow you to add reference counter to objects.
 * https://www.kernel.org/doc/html/latest/core-api/kref.html?highlight=kref
 */


#include <linux/err.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/kref.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/sched.h> //
#include <linux/slab.h>  // kmalloc(), kfree().
#include <linux/string.h>

#include "thunder_cat.h"


struct thunder_cat *panthro;
EXPORT_SYMBOL(panthro);

static void release_thunder_cat(struct kref *ref)
{
	struct thunder_cat *td = container_of(ref, struct thunder_cat, refcount);
	pr_info("%s: release_thunder_cat() is releasing thunder cat %s\n",
			KBUILD_MODNAME, td->name);
	thunder_cat_print(td);

	kfree(td);
}

static int __init krefs_module_init(void)
{
	// struct task_struct *task;

	pr_info("%s: Loading the module %s\n", KBUILD_MODNAME, KBUILD_MODNAME);

	panthro = kmalloc(sizeof(*panthro), GFP_KERNEL);
	if (!panthro) {
		return -ENOMEM;
	}

	thunder_cat_init(panthro, "Panthro", 30);

	thunder_cat_print(panthro);

	// Create a pthread and pass the thunder cat reference.


	// Increment the ref counter.
	// kref_get(&panthro->refcount);

	return 0;
}

static void __exit krefs_module_exit(void)
{
	pr_info("%s: Exiting the module %s\n", KBUILD_MODNAME, KBUILD_MODNAME);

	thunder_cat_print(panthro);

	// Releasing the reference in this module.
	kref_put(&panthro->refcount, release_thunder_cat);
}

module_init(krefs_module_init);
module_exit(krefs_module_exit);

MODULE_AUTHOR("John");
MODULE_DESCRIPTION("LFD420: Use of krefs");
MODULE_LICENSE("GPL v2");




