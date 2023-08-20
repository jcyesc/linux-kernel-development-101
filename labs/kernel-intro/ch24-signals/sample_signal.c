/* Program that installs and raises a signal.
 *
 *   $ gcc sample_signal.c
 *   $ ./a.out
 *   Sending the BAT signal!
 *   Signal hasn't arrived at 0.
 *   Signal handler for [2]. It is over
 *   ^C^C^C
 *   Signal handler for [2]. It is over
 *   ^C^C
 *   ^C^C^C
 *   Signal handler for [2]. It is over
 *   Signal hasn't arrived at 1.
 *   Signal hasn't arrived at 2.
 *   Signal hasn't arrived at 3.
 *   Signal handler for [2]. It is over
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void signal_handler(int sig) {
	printf("Signal handler for [%d]. It is over\n", sig);

	sleep(3);

	// exit(0);
}

int main() {
	printf("Sending the BAT signal!\n");

	/* Installing the signal. */
	signal(SIGINT, signal_handler);


	for (int i = 0; i < 20; i++) {
		printf("Signal hasn't arrived at %d.\n", i);

		if (i % 3 == 0) {
			/* Raising the signal. */
			raise(SIGINT);
		}
	}

	return 0;
}

