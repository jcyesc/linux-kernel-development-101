// SPDX-License-Identifier: GPL-2.0
/*
 * Module to print ARM64 processor information such as:
 *
 *   - Memory info (granule size, vm address size, etc).
 *   - Cache information.
 *   - Aarch64 exception handling
 *   - System register information.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": %s() " fmt, __func__

#include <linux/init.h>
#include <linux/module.h>

#ifdef CONFIG_ARM64_CACHE_INFO
#define CACHE_MSG "CONFIG_ARM64_CACHE_INFO is set\n"
#else
#define CACHE_MSG "CONFIG_ARM64_CACHE_INFO is NOT set\n"
#endif

static int __init arm64info_init(void)
{
	pr_info("ARM64 info\n");

#ifdef CONFIG_ARM64_MEMORY_INFO
	pr_info("CONFIG_ARM64_MEMORY_INFO is set\n");
#else
	pr_info("CONFIG_ARM64_MEMORY_INFO is NOT set\n");
#endif

	pr_info(CACHE_MSG);

#ifdef CONFIG_ARM64_EXCEPTION_INFO
	pr_info("CONFIG_ARM64_EXCEPTION_INFO is set\n");
#endif

#ifdef CONFIG_ARM64_SYSTEM_REG_INFO
	pr_info("CONFIG_ARM64_SYSTEM_REG_INFO is set\n");
#endif

	return 0;
}

static void __exit arm64info_exit(void)
{
	pr_info("Unloading the module\n");
}

module_init(arm64info_init);
module_exit(arm64info_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("ARM64 info");
MODULE_LICENSE("GPL v2");

