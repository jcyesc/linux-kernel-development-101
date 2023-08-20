/* **************** LFD420:5.14 s_24/lab_sigorder.c **************** */
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
 * Examining Signal Priorities.
 *
 * In the below, do not send or handle either of the signals SIGKILL
 * or SIGSTOP.
 *
 * Write a C program that includes a signal handler that can handle
 * any signal. The handler should avoid making any system calls (such
 * as those that might occur doing I/O).
 *
 * The handler should simply store the sequence of signals as they
 * come in, and update a counter array for each signal that indicates
 * how many times the signal has been handled.
 *
 * The program should begin by suspending processing of all signals
 * (using sigprocmask().
 *
 * It should then install the new set of signal handlers (which can be
 * the same for all signals, registering them with the sigaction()
 * interface.
 *
 * Then the program should send every possible signal to itself multiple
 * times, using the raise() function.
 *
 * Signal processing should be resumed, once again using sigprocmask).
 *
 * Before completing, the program should print out statistics
 * including:
 *
 *    - The total number of times each signal was received.
 *
 *    - The order in which the signals were received, noting each time the total
 *      number of times that signal had been received up to that point.
 *
 * Note the following items:
 *
 *   If more than one of a given signal is raised while the process
 *   has blocked it, does the process receive it multiple times?
 *
 *   Are all signals received by the process, or are some handled
 *   before they reach it?
 *
 *   What order are the signals received in?
 *
 * If you are running KDE as your desktop environment, you may find
 * signal 32 blocked.  One signal, SIGCONT (18 on x86) may not get
 * through; can you figure out why?
 *
 * It appears that signals 32 and 33 can not be blocked and will cause
 * your program to fail.  Even though header files indicate
 * SIGRTMIN=32, the command kill -l indicates SIGRTMIN=34.
 *
 * Avoid sending these signals.
 *
 * Note: SIGCONT (To be read in conjunction with SIGSTOP.) If a process has been
 * paused by sending it SIGSTOP then sending SIGCONT to the process wakes it up
 * again (“continues” it).
 *
 *    $ gcc lab_sigorder.c
 *    $ ./a.out
 *
 *    Installing signal handler and Raising signal for signal number:
 *
 *       1   2   3   4   5   6   7   8  --  10  11  12  13  14  15  16
 *      17  18  --  20  21  22  23  24  25  26  27  28  29  30  31  --
 *      --  34  35  36  37  38  39  40  41  42  43  44  45  46  47  48
 *      49  50  51  52  53  54  55  56  57  58  59  60  61  62  63  64
 *
 *    Signal  Number(Times Processed)
 *    --------------------------------------------
 *       1:  1     2:  1     3:  1     4:  1     5:  1     6:  1     7:  1     8:  1
 *       9:  0    10:  1    11:  1    12:  1    13:  1    14:  1    15:  1    16:  1
 *      17:  1    18:  0    19:  0    20:  1    21:  1    22:  1    23:  1    24:  1
 *      25:  1    26:  1    27:  1    28:  1    29:  1    30:  1    31:  1    32:  0
 *      33:  0    34:  3    35:  3    36:  3    37:  3    38:  3    39:  3    40:  3
 *      41:  3    42:  3    43:  3    44:  3    45:  3    46:  3    47:  3    48:  3
 *      49:  3    50:  3    51:  3    52:  3    53:  3    54:  3    55:  3    56:  3
 *      57:  3    58:  3    59:  3    60:  3    61:  3    62:  3    63:  3    64:  3
 *
 *
 *    History: Signal  Number(Count Processed)
 *    --------------------------------------------
 *
 *       4(1)   5(1)   7(1)   8(1)  11(1)  31(1)   1(1)   2(1)
 *       3(1)   6(1)  10(1)  12(1)  13(1)  14(1)  15(1)  16(1)
 *      17(1)  20(1)  21(1)  22(1)  23(1)  24(1)  25(1)  26(1)
 *      27(1)  28(1)  29(1)  30(1)  34(1)  34(2)  34(3)  35(1)
 *      35(2)  35(3)  36(1)  36(2)  36(3)  37(1)  37(2)  37(3)
 *      38(1)  38(2)  38(3)  39(1)  39(2)  39(3)  40(1)  40(2)
 *      40(3)  41(1)  41(2)  41(3)  42(1)  42(2)  42(3)  43(1)
 *      43(2)  43(3)  44(1)  44(2)  44(3)  45(1)  45(2)  45(3)
 *      46(1)  46(2)  46(3)  47(1)  47(2)  47(3)  48(1)  48(2)
 *      48(3)  49(1)  49(2)  49(3)  50(1)  50(2)  50(3)  51(1)
 *      51(2)  51(3)  52(1)  52(2)  52(3)  53(1)  53(2)  53(3)
 *      54(1)  54(2)  54(3)  55(1)  55(2)  55(3)  56(1)  56(2)
 *      56(3)  57(1)  57(2)  57(3)  58(1)  58(2)  58(3)  59(1)
 *      59(2)  59(3)  60(1)  60(2)  60(3)  61(1)  61(2)  61(3)
 *      62(1)  62(2)  62(3)  63(1)  63(2)  63(3)  64(1)  64(2)
 *      64(3)
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUMSIGS 64

/* prototypes of locally-defined signal handlers */

void (sig_handler) (int);

int sig_count[NUMSIGS + 1];	/* counter for signals received */
volatile static int line = 0;
volatile int signumbuf[6400], sigcountbuf[6400];

int main(int argc, char *argv[])
{
	sigset_t sigmask_new, sigmask_old;
	struct sigaction sigact, oldact;
	int signum;
	int rc;
	int i;

	/* block all possible signals */
	rc = sigfillset(&sigmask_new);
	rc = sigprocmask(SIG_SETMASK, &sigmask_new, &sigmask_old);

	/* Assign values to members of sigaction structures */
	memset(&sigact, 0, sizeof(struct sigaction));
	sigact.sa_handler = sig_handler;	/* we use a pointer to a handler */
	sigact.sa_flags = 0;	/* no flags */
	/* VERY IMPORTANT */
	sigact.sa_mask = sigmask_new;	/* block signals in the handler itself  */

	/*
	 * Now, use sigaction to create references to local signal
	 * handlers and raise the signal to myself
	 */

	printf
	    ("\nInstalling signal handler and Raising signal for signal number:\n\n");
	for (signum = 1; signum <= NUMSIGS; signum++) {
		if (signum == SIGKILL || signum == SIGSTOP || signum == 32
		    || signum == 33) {
			printf("  --");
		} else {
			sigaction(signum, &sigact, &oldact);
			/* send the signal 3 times! */
			rc = 0;
			rc += raise(signum);
			rc += raise(signum);
			rc += raise(signum);
			if (rc)
				printf("Failed on Signal %d\n", signum);
			else
				printf("%4d", signum);
		}
		if (signum % 16 == 0)
			printf("\n");
	}

	fflush(stdout);

	/* restore original mask */
	rc = sigprocmask(SIG_SETMASK, &sigmask_old, NULL);

	printf("\nSignal  Number(Times Processed)\n");
	printf("--------------------------------------------\n");
	for (i = 1; i <= NUMSIGS; i++) {
		printf("%4d:%3d  ", i, sig_count[i]);
		if (i % 8 == 0)
			printf("\n");
	}
	printf("\n");

	printf("\nHistory: Signal  Number(Count Processed)\n");
	printf("--------------------------------------------\n");
	for (i = 0; i < line; i++) {
		if (i % 8 == 0)
			printf("\n");
		printf("%4d(%1d)", signumbuf[i], sigcountbuf[i]);
	}
	printf("\n");

	exit(EXIT_SUCCESS);
}

void sig_handler(int sig)
{
	sig_count[sig]++;
	signumbuf[line] = sig;
	sigcountbuf[line] = sig_count[sig];
	line++;
}
