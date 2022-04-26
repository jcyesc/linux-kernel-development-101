/* **************** LFD420:5.14 s_17/lab_cfs.c **************** */
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
 * Simulating the CFS scheduler.
 *
 * Write a program that simulates the operation of the upcoming CFS
 * scheduler.
 *
 * You should consider a fixed number of equal intrinsic priority
 * tasks.  At each scheduling step, the task which has the most fair
 * time due to it should be scheduled to run.
 *
 * The time the task should run (after which the scheduler is run
 * again) should vary up to some maximum amount determined by a randon
 * number generator.
 *
 * Keep track of the cumulative CPU time for each task.
 *
 * After each step the fair time missed for each task should be
 * updated to be used for calculating the new relative priority for
 * the next scheduling decision.
 *
 * You may want to limit the maximum number of scheduling steps.
 *
 * Evaluate how well the load is balanced as time proceeds.
 *
 * Note: Try first with:
 *
 *    #define PRINTF printf
 *
 *    int ktop = 15;
 *
 * So you can see the progress of the algorithm.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int num_tasks = 10;          /* how many tasks */
const int ktime_units = 10;  /* how much time units to give at max
                              * the actual time given is tim * ktime_units where
                              * time < num_tasks so we can do integer arithmetic
                              */
int modprint = 1000;         /* how often to print out as we go; */
int *prio;                   /* priority vector */
int *total;                  /* total time spent vector */
int ktop = 1000000;          /* maximum number of simulations */

static int k;

/* Try running the algorithm with (k % modprint != 0) so
 * you can see the algorith clearly.
 */
#define PRINTF if (k % modprint == 0) printf

/*
 * Return the task that has the highest priority. The CFS would use a Red Black
 * Tree to get the next task instead of iterating throught the whole list.
 */
int schedule(void)
{
	int j;
	int tsk = 0;
	int test = -1;

	for (j = 0; j < num_tasks; j++) {
		if (prio[j] > test) {
			test = prio[j];
			tsk = j;
		}
	}

	return tsk;
}

void update_priorities_and_totals(int tsk, int time)
{
	int j;
	int timeslot;
	int fairtime;	/* note fairtime will always be time. */

	timeslot = time * num_tasks;
	fairtime = timeslot / num_tasks;
	for (j = 0; j < num_tasks; j++)
		prio[j] += fairtime;

	prio[tsk] += -timeslot;	/* don't make it not be negative */
	total[tsk] += time;
}

void printit(int tsk, int tim)
{
	int j, diff, totave = 0, disp = 0;

	PRINTF("\nk=%4d, tsk=%d, time slice=%d,\n", k, tsk, tim);
	for (j = 0; j < num_tasks; j++) {
		PRINTF("[%2d]=%3d  ", j, prio[j]);
		totave += total[j];
	}
	PRINTF("\n");

	totave = totave / num_tasks;
	for (j = 0; j < num_tasks; j++) {
		diff = total[j] - totave;
		PRINTF("%8d  ", diff);
		if (diff > 0)
			disp += diff;
		else
			disp += -diff;
	}

	PRINTF("\n average=%d, dispersion =%d\n", totave, disp / num_tasks);
}

int main(int argc, char *argv[])
{
	int tsk;
	int tim;
	int i;

	if (argc > 1)
		num_tasks = atoi(argv[1]);
	if (argc > 2)
		modprint = atoi(argv[2]);
	if (argc > 3)
		ktop = atoi(argv[3]);

	srand(getpid());
	prio = calloc(num_tasks, sizeof(int));
	total = calloc(num_tasks, sizeof(int));

	for (k = 0; k < ktop; k++) {
		/* select how long to run */
		tim = rand() % ktime_units + 1;
		/* select which process to run */
		tsk = schedule();
		/* print results */
		printit(tsk, tim);
		/* update priorities and total times */
		update_priorities_and_totals(tsk, tim);
	}

	/* Print final totals */
	printf("\n===============================\n");
	printf("Task  Priority  Total Execution");
	printf("\n===============================\n");
	for (i = 0; i < num_tasks; i++) {
		printf("%4d  %8d      %d\n", i, prio[i], total[i]);
	}

	exit(EXIT_SUCCESS);
}
