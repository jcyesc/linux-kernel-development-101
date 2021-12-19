/*
 *  Prints the cpu that is executing this program.
 *
 *  Compile with:
 *
 *  $ aarch64-linux-gnu-gcc -o get_cpu get_cpu.c
 *
 *  Execute it with
 *
 *  $ taskset -c 3 ./get_cpu
 *  The CPU executing this program is 3
 *  The CPU executing this program is 3
 *  The CPU executing this program is 3
 *
 * If you try to set the affinity of the get_cpu program to another CPU, the PID
 * changes and it could change the CPU where the program is running:
 *
 * $ taskset -pc 1 13252
 *
 * where 13252 is the PID of get_cpu.
 */


/**
 * This define is necessary to avoid the warning below:
 *
 * get_cpu.c: In function ‘main’:
 * get_cpu.c:25:12: warning: implicit declaration of function ‘sched_getcpu’
 * [-Wimplicit-function-declaration] int cpu = sched_getcpu();
 *
 * Note: The directive has to be at the top of the file.
 */
#define _GNU_SOURCE

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(void)
{
	int cpu = sched_getcpu();
	int counter = 0;
	unsigned int seconds = 2;

	while (counter++ < 1000) {
		printf("The CPU executing this program is %d\n", cpu);
		sleep(seconds);
	}

	exit(EXIT_SUCCESS);
}


