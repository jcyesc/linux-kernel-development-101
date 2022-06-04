// SPDX-License-Identifier: GPL-2.0
/*
 * Module that shows how to use kfifo data structure.
 *
 * - Declare kfifo
 * - Create a queue
 * - Enqueue data
 * - Dequeue data
 * - Reset and destroy the queue
 *
 * More information in:
 *
 * https://elixir.bootlin.com/linux/v5.18.1/source/include/linux/kfifo.h
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": %s() " fmt, __func__

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kfifo.h>
#include <linux/module.h>
#include <linux/printk.h>

struct kfifo kfifo;

static inline void enqueue(void)
{
	int i;

	for (i = 23; i < 50; i++)
		kfifo_in(&kfifo, &i, sizeof(i));
}

static inline void peek(void)
{
	int val;
	int ret;

	ret = kfifo_size(&kfifo);
	pr_info("Size of the kfifo is %d", ret);

	ret = kfifo_out_peek(&kfifo, &val, sizeof(val));

	if (ret != sizeof(val)) {
		pr_err("Number of bytes read don't match. Expected %lu, got %d",
			sizeof(val), ret);
		return;
	}

	pr_info("Value read from kfifo %d\n", val);
}

/**
 * Return 0 on success, otherwise it returns a negative number.
 */
static inline int dequeue(void)
{
	while (!kfifo_is_empty(&kfifo)) {
		int val;
		int ret;

		// Read one integer at a time.
		ret = kfifo_out(&kfifo, &val, sizeof(val));
		if (ret != sizeof(val)) {
			pr_err("Number of bytes read don't match. Expected %lu, got %d",
				sizeof(val), ret);
			return -EINVAL;
		}

		pr_info("%d ", val);
	}

	return 0;
}

static int __init kfifo_mod_init(void)
{
	int ret;

	pr_info("Initializing module");

	// The kfifo size has to be multiple of 2.
	ret = kfifo_alloc(&kfifo, PAGE_SIZE, GFP_KERNEL);
	if (ret) {
		pr_err("Failed to allocate kfifo");
		return ret;
	}

	enqueue();
	peek();

	ret = dequeue();
	if (ret) {
		pr_err("Failed dequeueing!");
		return ret;
	}

	return 0;
}

static void __exit kfifo_mod_exit(void)
{
	pr_info("Exiting module");

	kfifo_reset(&kfifo);
	kfifo_free(&kfifo);
}

module_init(kfifo_mod_init);
module_exit(kfifo_mod_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("kfifo library usage");
MODULE_LICENSE("GPL v2");

