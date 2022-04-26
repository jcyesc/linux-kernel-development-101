
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void signal_handler(int sig) {
	printf("Signal handler for [%d]. It is over\n", sig);

	exit(0);
}

int main() {
	printf("Sending the BAT signal!\n");

	/* Installing the signal. */
	signal(SIGINT, signal_handler);

	/* Raising the signal. */
	raise(SIGINT);

	for (int i = 0; i < 200; i++) {
		printf("Signal hasn't arrived at %d.\n", i++);
	}

	return 0;
}

