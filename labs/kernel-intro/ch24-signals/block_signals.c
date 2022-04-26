/*
 * This program blocks all the signals for this program when it is executing,
 * so Ctrl-c keyboard sequences are ignored.
 *
 * To run, execute the commands below:
 *
 *  $ ./a.out
 *  Sleep 5 seconds, try Ctrl-c
 *  ^CContinue execution
 *  $ ./a.out
 *  Sleep 5 seconds, try Ctrl-c
 *  Continue execution
 *  This is a greeting.
 *  This is a greeting.
 *  This is a greeting.
 *  ^C
 *
 *  In another shell you can run the comand below to examine the masks of
 *  pending, blocked, ignored and caught signals for processes on your system:
 *
 *  $ id
 *  uid=1000(parallels) gid=1000(parallels) groups=1000(parallels),20(dialout),27(sudo)
 *  $ ps axs
 */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	int rc;
	sigset_t sigsus;
	sigset_t oldset;

	/* Block all possible signals. */
	rc = sigfillset(&sigsus);
	if (rc) {
		printf("sigfillset() failed!\n");
		return -1;
	}

	rc = sigprocmask(SIG_SETMASK, &sigsus, &oldset);
	if (rc) {
		printf("sigprocmask() failed|\n");
		return -1;
	}

	printf("Sleep 5 seconds, try Ctrl-c\n");
	sleep(5);
	printf("Continue execution\n");

	/* Restore original mask. */
	rc = sigprocmask(SIG_SETMASK, &oldset, NULL);
	if (rc) {
		printf("sigprocmask() failed!\n");
		return -1;
	}

	/* The process should terminate before the next loop if you sent
	 * a Ctrl+c while the signals were blocked.
	 */

	/* Do something pointless, forever. */
	for(;;) {
		printf("This is a greeting.\n");
		sleep(1);
	}

	return 0;
}
