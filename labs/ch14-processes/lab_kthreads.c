/* **************** LFD420:5.14 s_14/lab_kthreads.c **************** */
/*
 * The code herein is: Copyright the Linux Foundation, 2021
 *
 * This Copyright is retained for the purpose of protecting free
 * redistribution of source.
 *
 *     URL:    https://training.linuxfoundation.org
 *     email:  info@linuxfoundation.org
 *
 * This code is distributed under Version 2 of the GNU General Public
 * License, which you should have received with the source.
 *
 */
/*
 * Creating Per Cpu Threads
 *
 * Create a kernel thread on each cpu during your intialization, and
 * have it go to sleep, to be woken up and killed during your exit
 * function.
 *
 * The easiest way to induce a short sleep is with:
 *
 *    #include <linux/delay.h>
 *    void msleep (unsigned int milliseconds);
 *    unsigned long msleep_interruptible (unsigned int milliseconds);
 *
 * If msleep_interruptible() returns before the sleep has finished
 * (because of a signal) it returns the number of milliseconds left in
 * the requested sleep period.
 *
 * The easiest way to iterate over all cpu's is to use the macro:
 *
 *     for_each_online_cpu(cpu){
 *          .... operate on cpu
 *     }
 *
 * Print out which cpu each thread is running on.
 *
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/delay.h>

/* NR_CPUS is defined in kernel config as maximum possible,
 * not the actual numdber
 */
static struct task_struct *tsk[NR_CPUS];

static void print_cpu(char *s)
{
	preempt_disable();
	pr_info("%s cpu=%d.\n", s, smp_processor_id());
	preempt_enable();
}

static int thr_fun(void *thr_arg)
{
	do {
		print_cpu("SLEEP in Thread Function ");
		msleep_interruptible(2000);
		print_cpu("msleep over in Thread Function");
	} while (!kthread_should_stop());
	return 0;
}

/* Note there is a leak bug here because the init function
   does not clean up properly if kthread_create() fails on
   any iteration other than the first. This could be fixed
   (say in the exit function) but is very hard to trigger
   and we want to keep the code simple Thanks to Frank
   Edwards for pointing this out */

static int __init my_init(void)
{
	int i;

	print_cpu("Loading module");
	for_each_online_cpu(i) {
		tsk[i] = kthread_create(thr_fun, NULL, "thr_fun/%d", i);
		if (!tsk[i]) {
			pr_info("Failed to generate a kernel thread\n");
			return -1;
		}
		kthread_bind(tsk[i], i);
		pr_info("About to wake up and run the thread for cpu=%d\n", i);
		wake_up_process(tsk[i]);
		pr_info("Staring thread for cpu %d", i);
		print_cpu("on");
	}
	return 0;
}

static void __exit my_exit(void)
{
	int i;

	for_each_online_cpu(i) {
		pr_info(" Kill Thread %d", i);
		kthread_stop(tsk[i]);
		print_cpu("Kill was done on ");
	}
}

module_init(my_init);
module_exit(my_exit);
MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD420:5.14 s_14/lab_kthreads.c");
MODULE_LICENSE("GPL v2");
