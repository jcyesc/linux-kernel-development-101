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

/**
 * This define is necessary to avoid the warnings related to the use of kernel
 * headers in the user space.
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


/*
 * This variable will be shared between child and parent. No locks will be used
 * so corruption can happen.
 */
int shared_value;

/*
 * The handler function that the child thread will execute.
 */
int handler(void *data)
{
	int counter;

	printf("child: Thread starts execution: ppid[%d], pid[%d]\n", getppid(), getpid());

	for (counter = 0; counter < 7; counter++) {
		shared_value += 7;
		sleep(1);
		/* Note: the ppid and pid are the same for parent and proccess. What
		 * it is different is tid.
		 */
		printf("child:  ppid[%d], pid[%d], shared_value[%d]\n",
				getppid(), getpid(), shared_value);
	}

	printf("child: Thread exits: ppid[%d], pid[%d]\n", getppid(), getpid());
	exit(EXIT_SUCCESS);
}

void * create_stack()
{
	// The stacks grows from higher addresses to lower addresses.
	void *stack;
	int page_size = getpagesize();
	int stack_size = 4 * page_size;

	/* The stack address has to be aligned properly. In ARMv8 AArch64, the stack
	 * has to be 16-byte aligned, in ARMv8 AArch32 has to be 4-byte aligned.
	 */
	if (posix_memalign(&stack, page_size, stack_size)) {
		perror("posix_memalign failed");
		exit(EXIT_FAILURE);
	}

	// The stacks grows from higher addresses to lower addresses. That is the
	// reason that with have to set the stack pointer to the higher address.
	stack = (char *)stack + stack_size;

	printf("parent: Child stack [%p], child stack size [%d], Page size [%d]\n",
			stack, stack_size, page_size);

	return stack;
}

int main(int argc, char *argv[])
{
	void *stack;
	int tid;
	int counter;

	printf("parent: Parent ppid[%d], pid[%d]\n", getppid(), getpid());

	// Set up new children thread.
	stack = create_stack();
	tid = clone(handler,
			stack,
			CLONE_VM | // Parent and child process share memory space.
			CLONE_FS | // Parent and child share filesystem information.
			CLONE_FILES | // Parent and child share the same table of file descriptors.
			CLONE_SYSVSEM | // Share system V SEM_UNDO semantics
			CLONE_SIGHAND | // Parent and child share the same set of signal handlers.
			CLONE_THREAD, // The cloned process will have the same thread group as the cloner.
			NULL);
	printf("parent: Child tid[%d]\n", tid);

	if (tid < 0) {
		perror("clone failed");
		exit(EXIT_FAILURE);
	}

	for (counter = 0; counter < 4; counter++)
	{
		shared_value += 11;
		sleep(1);
		printf("parent: ppid[%d], pid[%d], shared_value[%d]\n",
				getppid(), getpid(), shared_value);
	}

	printf("parent: Parent exits\n");

	/* The stack is not free since the child thread might be still running. */
	exit(EXIT_SUCCESS);
}

