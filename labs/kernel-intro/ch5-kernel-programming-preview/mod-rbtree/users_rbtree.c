// SPDX-License-Identifier: GPL-2.0
/*
 * Module that shows how to use the red black tree library in Linux.
 *
 * - define root tree
 * - insert nodes
 * - find nodes
 * - traverse tree
 */

#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/rbtree.h>
#include <linux/types.h>

#define USER_NAME_SIZE 40
struct usertype {
	unsigned int id;
	char name[USER_NAME_SIZE];
	struct rb_node user_rb;
};

#define NUM_USERS 6
static struct usertype users[NUM_USERS] = {
		{.id = 26, .name = "root",   .user_rb = {}},
		{.id = 25, .name = "king",   .user_rb = {}},
		{.id = 24, .name = "hacker", .user_rb = {}},
		{.id = 23, .name = "super",  .user_rb = {}},
		{.id = 22, .name = "travis", .user_rb = {}},
		{.id = 21, .name = "alpha",  .user_rb = {}},
};

static struct rb_root root = RB_ROOT;

static struct usertype *rb_search_user(unsigned int id)
{
	struct rb_node *n = root.rb_node;

	while (n) {
		struct usertype *user = rb_entry(n, struct usertype, user_rb);

		if (id < user->id)
			n = n->rb_left;
		else if (id > user->id)
			n = n->rb_right;
		else
			return user;
	}

	return NULL;
}

/*
 * Inserts a node in the red black tree.
 *
 * Returns a pointer to struct user if it is already in the tree, otherwise it
 * returns NULL.
 */
static struct usertype *rb_insert_user(struct usertype *newuser)
{
	struct rb_node **p = &root.rb_node;
	struct rb_node *parent = NULL;
	struct usertype *user;
	unsigned int id = newuser->id;

	while (*p) {
		parent = *p;
		user = rb_entry(parent, struct usertype, user_rb);

		if (id < user->id)
			p = &(*p)->rb_left;
		else if (id > user->id)
			p = &(*p)->rb_right;
		else
			return user;
	}

	rb_link_node(&newuser->user_rb, parent, p);
	rb_insert_color(&newuser->user_rb, &root);

	return NULL;
}

static inline void traverse_rb(void)
{
	struct usertype *user, *next;

	pr_info("Traversing in postorder");

	rbtree_postorder_for_each_entry_safe(user, next, &root, user_rb) {
		pr_info("user [id: %d, name: %s]", user->id, user->name);
	}
}

static int __init users_rbtree_init(void)
{
	unsigned int i;
	struct usertype *user;
	unsigned int id;

	pr_info("Initializing rbtree module");

	pr_info("Inserting users\n");
	for (i = 0; i < NUM_USERS; i++)
		rb_insert_user(&users[i]);

	traverse_rb();

	pr_info("Finding users\n");
	for (i = 0; i < NUM_USERS + 5; i++) {
		id = i + 20;
		user = rb_search_user(id);
		if (user)
			pr_info("user [id: %d, name: %s]", user->id, user->name);
		else
			pr_info("user with id=%d was not found", id);
	}

	return 0;
}

static void __exit users_rbtree_exit(void)
{
	pr_info("Exiting rbtree module");
}

module_init(users_rbtree_init);
module_exit(users_rbtree_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Red black trees library usage");
MODULE_LICENSE("GPL v2");

