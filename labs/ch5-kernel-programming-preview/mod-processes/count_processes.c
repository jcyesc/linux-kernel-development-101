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

static int count_number_processes(void)
{
	struct task_struct *task;
	int counter = 0;

	pr_info("Name\t\ttgid\t\tpid");
	for_each_process(task)
	{
		counter++;
		pr_info("%s\t\t%d\t\t%d", task->comm, task->tgid, task->pid);
	}

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

MODULE_AUTHOR("John");
MODULE_DESCRIPTION("Processes example");

