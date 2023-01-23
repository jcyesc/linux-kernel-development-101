// SPDX-License-Identifier: GPL-2.0
/*
 * Out of tree module to print ARM64 processor information such as:
 *
 *   - Memory info (granule size, vm address size, etc).
 *   - Cache information.
 *   - Aarch64 exception handling
 *   - System register information.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>

#include "arm64_cache.h"
#include "arm64_mm.h"


static int __init arm64_conf_info_init(void)
{
	pr_info("Loading module");

	mm_print_aa64mmfr0_el1_reg();
	cache_print_clidr_el1();

	return 0;
}

static void __exit arm64_conf_info_exit(void)
{
	pr_info("Unloading module");
}

module_init(arm64_conf_info_init);
module_exit(arm64_conf_info_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Prints ARM64 config info");
MODULE_LICENSE("GPL v2");

