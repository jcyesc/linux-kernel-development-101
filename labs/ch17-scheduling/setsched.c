/* **************** LFD420:5.14 s_17/setsched.c **************** */
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
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define DEATH(mess) { perror(mess); exit(errno); }

/* utility to print out policy */
void printpolicy(int policy)
{
	/* SCHED_NORMAL = SCHED_OTHER in user-space */

	if (policy == SCHED_OTHER)
		printf("policy = SCHED_OTHER = %d\n", policy);
	if (policy == SCHED_FIFO)
		printf("policy = SCHED_FIFO = %d\n", policy);
	if (policy == SCHED_RR)
		printf("policy = SCHED_RR = %d\n", policy);
}

int main(void)
{
	int policy;
	struct sched_param p;
	/* If pid equals zero, the scheduling policy and parameters of
     * the calling thread will be set.
     *
     * https://man7.org/linux/man-pages/man2/sched_setscheduler.2.html
     */
	pid_t pid = 0;

	/* obtain scheduling policy */

	policy = sched_getscheduler(pid);
	printpolicy(policy);

	/* reset scheduling policy */

	printf("\nTrying sched_setscheduler...\n");
	policy = SCHED_FIFO;
	printpolicy(policy);
	p.sched_priority = 50;

    /*
     * To change the scheduler policy and priority, it is necessary to have
     * root privileges.
     */
	if (sched_setscheduler(pid, policy, &p))
		DEATH("sched_setscheduler:");
	printf("p.sched_priority = %d\n", p.sched_priority);

	exit(EXIT_SUCCESS);
}

