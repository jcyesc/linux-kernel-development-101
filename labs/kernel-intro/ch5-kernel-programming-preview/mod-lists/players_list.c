// SPDX-License-Identifier: GPL-2.0
/*
 * Module that shows how to use:
 *
 * - list in include/linux/list.h
 * - kmalloc/kfree in include/linux/slab.h
 */

#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/slab.h>


struct player {
	int id;
	char name[40];
	struct list_head node;
};

// Initializing the list.
LIST_HEAD(players_list);

static int __init players_list_init(void)
{
	int counter;
	struct player *p;
	struct list_head *list_ptr;

	pr_info("players_list_init(void)");

	for (counter = 0; counter < 5; counter++) {
		p = kmalloc(sizeof(struct player), GFP_KERNEL);
		p->id = counter + 1;
		sprintf(p->name, "player_%d", p->id);
		list_add_tail(&p->node, &players_list);
	}

	list_for_each(list_ptr, &players_list) {
		p = list_entry(list_ptr, struct player, node);

		pr_info("id: %d, name: %s\n", p->id, p->name);
	}

	return 0;
}

static void __exit players_list_exit(void)
{
	struct list_head *list_pos;
	struct list_head *list_tmp;
	struct player *p;

	pr_info("players_list_exit(void)");

	list_for_each_safe(list_pos, list_tmp, &players_list) {
		p = list_entry(list_pos, struct player, node);

		pr_info("Removing node from the list\n");
		list_del(list_pos);

		pr_info("Freeing id: %d, name: %s\n", p->id, p->name);
		kfree(p);
	}

	if (list_empty(&players_list)) {
		pr_info("players_list is empty!");
	}

	pr_info("Checking if there are elements:");
	list_for_each(list_pos, &players_list) {
		p = list_entry(list_pos, struct player, node);

		pr_info("id: %d, name: %s\n", p->id, p->name);
	}
}

module_init(players_list_init);
module_exit(players_list_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Linked list example");
MODULE_LICENSE("GPL v2");

