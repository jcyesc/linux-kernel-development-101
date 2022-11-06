// SPDX-License-Identifier: GPL-2.0
/*
 * Module to print information about processes.
 *
 * For each task, printout the thread group ID, process ID, state, command line.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/sched.h> // struct task_struct
#include <linux/sched/signal.h> // for_each_process() macro


// The __initdata macro indicates the kernel that the variable should be removed
// from memory after initializing.
static int count __initdata = 0 ;

// The __init macro indicates the kernel that the function should be removed
// from memory after initializing.
static int __init processes_info_init(void)
{
	struct task_struct *task, *process;

	pr_info("----- processes_info module (for_each_proccess(task) -----");
	for_each_process(task)
	{
		count++;
		pr_info("%s\t\t%d\t\t%d", task->comm, task->tgid, task->pid);
	}

	pr_info("Total processes: %d", count);

	pr_info("----- processes_info module (for_each_process_thread(p, t) -----");
	count = 0;
	for_each_process_thread(process, task)
	{
		pr_info("tgid=%d, pid=%d, comm=%s",
			task->tgid, task->pid, task->comm);
	}

	pr_info("Total threads: %d", count);

	return 0;
}

// The __exit macro does not do much except group all such labeled material
// together in the executable in an area not likely to be cached.
static void __exit processes_info_exit(void)
{
	pr_info("Exit from processes_info module!");
}

module_init(processes_info_init);
module_exit(processes_info_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Processes info example");
MODULE_LICENSE("GPL v2");

