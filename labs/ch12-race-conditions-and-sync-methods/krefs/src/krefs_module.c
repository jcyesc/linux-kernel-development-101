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
#include <linux/sched.h> // task_struct
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
EXPORT_SYMBOL(release_thunder_cat);


static int thunder_cat_handler(void *td_data)
{
    struct thunder_cat *td = td_data;
    pr_info("thunder_cat_handler() has the thunder cat %s\n", td->name);
    thunder_cat_print(td);

    // Decrementing the kref counter.
    kref_put(&td->refcount, release_thunder_cat);

    return 0;
}

/*
 * Perform the tasks below:
 *
 * - Create a thunder cat and initialize the {@code panthro} reference with the
 *   new object.
 * - Keeps the {@code panthro->refcount} updated.
 * - Creates a kthread and pass the {@code panthro} reference.
 */
static int __init krefs_module_init(void)
{
	struct task_struct *task;
	int ret  =0;

	pr_info("%s: Loading the module %s\n", KBUILD_MODNAME, KBUILD_MODNAME);

	panthro = kmalloc(sizeof(*panthro), GFP_KERNEL);
	if (!panthro) {
		return -ENOMEM;
	}

	thunder_cat_init(panthro, "Panthro", 30);
	thunder_cat_print(panthro);

	// Incrememting the kref counter.
	kref_get(&panthro->refcount);
	task = kthread_run(thunder_cat_handler, panthro, "thunder_cat_handler");
	if (task == ERR_PTR(-ENOMEM)) {
		ret = -ENOMEM;
		// Decrementing the kref counter if there is an error creating kthread.
		kref_put(&panthro->refcount, release_thunder_cat);
	}

	return ret;
}

static void __exit krefs_module_exit(void)
{
	pr_info("%s: Exiting the module %s\n", KBUILD_MODNAME, KBUILD_MODNAME);

	thunder_cat_print(panthro);

	// Decrementing and releasing the reference in this module.
	kref_put(&panthro->refcount, release_thunder_cat);
}

module_init(krefs_module_init);
module_exit(krefs_module_exit);

MODULE_AUTHOR("John");
MODULE_DESCRIPTION("LFD420: Use of krefs");
MODULE_LICENSE("GPL v2");




