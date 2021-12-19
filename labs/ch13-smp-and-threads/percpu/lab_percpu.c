/* **************** LFD420:5.14 s_13/lab_percpu.c **************** */
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
 * Per Cpu Variables
 *
 * Write a trivial module that creates per-cpu variables, and modifies
 * and prints out their values.
 *
 * Try using both methods; a fixed allocation and a dynamical
 * allocation.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/percpu.h>

static DEFINE_PER_CPU(long, cpuvar) = 10;
static long __percpu *cpualloc;

static DEFINE_SPINLOCK(dumb_lock);
static int which_cpu(void)
{
	int cpu;

	spin_lock(&dumb_lock);
	cpu = smp_processor_id();
	spin_unlock(&dumb_lock);
	return cpu;
}

static int __init my_init(void)
{
	int cpu;

	pr_info("cpuvar=%ld\n", get_cpu_var(cpuvar)++);
	cpu = which_cpu();
	put_cpu_var(cpuvar);
	cpualloc = alloc_percpu(long);
	*per_cpu_ptr(cpualloc, cpu) = 1000;
	pr_info("Hello: module loaded at 0x%p\n", my_init);
	pr_info("Set variables on cpu:%d\n", cpu);
	return 0;
}

static void __exit my_exit(void)
{
	int cpu;

	pr_info("Now on cpu:%d\n", which_cpu());

	for_each_online_cpu(cpu) {
		pr_info("cpu:%d cpuvar=%ld per_cpu_ptr=%ld\n", \
			cpu, per_cpu(cpuvar, cpu), *per_cpu_ptr(cpualloc, cpu));
	}

	free_percpu(cpualloc);
	pr_info("Bye: module unloaded from 0x%p\n", my_exit);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD420:5.14 s_13/lab_percpu.c");
MODULE_LICENSE("GPL v2");
