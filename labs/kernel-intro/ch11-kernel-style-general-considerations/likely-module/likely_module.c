// SPDX-License-Identifier: GPL-2.0
/*
 * Module that shows and example of likely/unlikey
 *
 * To check that the likely/unlikely assumption is correct, the
 * kernel configuration:
 *
 *    CONFIG_PROFILE_ANNOTATED_BRANCHES=y`
 *
 * has to be selected.
 *
 * Once that the module is installed, we can check the assumption
 * using:
 *
 *    $ mount -t debugfs none /sys/kernel/debug
 *    $ cat /sys/kernel/debug/trace_stat/branch_annotated
 *    correct incorrect  %        Function                  File              Line
 *    ------- ---------  -        --------                  ----              ----
 *      0  5037841 100 pl011_tx_chars                 amba-pl011.c         1489
 *      0  5037841 100 pl011_tx_char                  amba-pl011.c         1427
 *      0    41328 100 on_null_domain                 fair.c               11202
 *      0    31418 100 trigger_load_balance           fair.c               11860
 *      0     6143 100 page_try_dup_anon_rmap         rmap.h               262
 *      0     4192 100 rcu_gp_cleanup                 tree.c               1726
 *      0     4192 100 rcu_gp_init                    tree.c               1515
 *      0     3655 100 mas_start                      maple_tree.c         1420
 *      0     2690 100 resume_user_mode_work          resume_user_mode.h   48
 *      0     2502 100 do_vmi_align_munmap            mmap.c               2454
 *      0     1065 100 secure_computing               seccomp.h            47
 *      0      877 100 security_cred_free             security.c           1770
 *      0      391 100 move_page_tables               mremap.c             560
 *      0      367 100 rcu_softirq_qs                 tree.c               246
 *
 * Note: Unfortunately, the out of tree modules are not shown in `branch_annotated`.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>


static int __init likely_module_init(void)
{
	int odd = 0;
	int even = 0;

	pr_info("Starting module");

	for (int i = 0; i < 10000000; i += 3) {
		if (likely(i % 2 == 0))
			even++;

		if (unlikely(i % 3 == 0))
			odd++;
	}

	pr_info("# odds = %d", odd);
	pr_info("# even = %d", even);

	return 0;
}

static void __exit likely_module_exit(void)
{
	pr_info("Exiting module");
}

module_init(likely_module_init);
module_exit(likely_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Example of likely/unlikely");
