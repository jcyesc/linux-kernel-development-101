// SPDX-License-Identifier: GPL-2.0
/**
 * Module to count the number of processes.
 *
 * This module prints the processes that are in the system when the module is
 * loaded and removed.
 *
 * Load the module with:
 *
 * $ sudo insmod count_processes.ko
 *
 * See the messages with `dmesg` command.
 *
 * Remove the module with:
 *
 * $ sudo rmmod count_processes
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/sched.h> // struct task_struct
#include <linux/sched/signal.h> // for_each_process
#include <linux/sched/task.h>

static int count_number_processes(void)
{
	struct task_struct *task;
	int counter = 0;

	pr_info("Name\t\ttgid\t\tpid");

	//	When the a task_struct it is used, the lasklist_lock should be used. The
	//	tasklist_lock hasn't been exported in the Kernel because it is not recommended
	//	to use it in modules. Below it is how it is supposed to be used outside modules.
	//	tasklist_lock is defined in https://elixir.bootlin.com/linux/v5.18.3/source/kernel/fork.c#L142
	//
	//  read_lock(&tasklist_lock);

	for_each_process(task)
	{
		//
		//	if (tsk)
		//		get_task_struct(tsk);  // Increase ref counter.
		counter++;
		pr_info("%s\t\t%d\t\t%d", task->comm, task->tgid, task->pid);
	}

	// read_unlock(&tasklist_lock);

	return counter;
}

static int __init count_processes_init(void)
{
	int nprocesses;

	pr_info("\nInit counting processes module!");

	nprocesses = count_number_processes();
	pr_info("\nNumber of processes when the module is init: %d", nprocesses);

	return 0;
}

static void __exit count_processes_exit(void)
{
	int nprocesses;

	pr_info("\nExit counting processes module!");

	nprocesses = count_number_processes();
	pr_info("\nNumber of processes when the module is exit: %d", nprocesses);
}

module_init(count_processes_init);
module_exit(count_processes_exit);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Processes example");
MODULE_LICENSE("GPL v2");

