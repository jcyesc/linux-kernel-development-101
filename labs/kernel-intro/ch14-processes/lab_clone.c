/* **************** LFD420:5.14 s_14/lab_clone.c **************** */
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
 * Cloning
 *
 * Write a short program that uses the clone() system call to create
 * parent and child threads that share all memory resources.
 *
 * Have a global variable and show that either thread can modify it
 * and the value can be seen by the other.
 *
 * Be careful with creation of the child thread's stack pointer, to
 * point to the top of the stack since it grows down.
 *
 * What happens if the child thread dies before the parent thread?
 *
 * Note you can use this as a nucleus of your own multi-threaded
 * programming method that does not use the standard threading
 * libraries.  You can also play with sharing file descriptors or not
 * etc.
 */

#define _GNU_SOURCE

#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/wait.h>

int param;

int slav(void *data)
{
	int j;

	printf("starting off child thread, pid=%d\n", getpid());
	for (j = 0; j < 10; j++) {
		param = j + 1000;
		sleep(1);
		printf("\nchild thread running: j=%d, param=%d secs\n", j,
		       param);
	}
	printf("\nChild thread saying goodbye\n");
	return 0;
}

int main(int argc, char **argv)
{
	int j, tid, pagesize, stacksize;
	void *stack;

	printf("starting off parent thread, pid=%d\n", getpid());

	pagesize = getpagesize();
	stacksize = 4 * pagesize;

	/* could probably just use malloc(), but this is safer */
	/* stack = (char *)memalign (pagesize, stacksize); */
	if (posix_memalign(&stack, pagesize, stacksize)) {
		perror("posix_memalign failed");
		exit(EXIT_FAILURE);
	}
	printf
	    ("Setting off a clone thread with stack (0x%p) stacksize = %d....",
	     stack, stacksize);
	tid =
	    clone(slav, (char *)stack + stacksize,
		  CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SYSVSEM |
		  CLONE_SIGHAND | CLONE_THREAD, NULL);
	printf(" with tid=%d\n", tid);
	if (tid < 0) {
		perror("clone failed");
		exit(EXIT_FAILURE);
	}

	/* could do a wait (&status) here if required */

	for (j = 0; j < 6; j++) {
		param = j;
		sleep(1);
		printf("\nparent thread running: j=%d, param=%d secs\n", j,
		       param);
	}
	printf("parent killitself\n");
	/* We shouldn't free(stack) here since the child using it is still running */
	exit(EXIT_SUCCESS);
}
