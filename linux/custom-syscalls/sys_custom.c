#include <linux/kernel.h>
#include <linux/syscalls.h>

/**
 * Defines a syscall that just print a message and returns
 * a number.
 */
SYSCALL_DEFINE0(custom)
{
	printk("sys_custom() system call in custom-syscalls/sys_custom.c\n");

	return 3;
}
