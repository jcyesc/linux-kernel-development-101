/*
 * Program that allocates memory in multiples of page size. After that, it
 * releases the memory.
 *
 * The intend of this program is to observe the process in /proc/pid/status
 * and see how the memory footprint grows and decrease as the programs execute.
 *
 * $ gcc -o mem_request mem_request.c
 * $ ./mem_request
 * PID [31764] Requesting 4096 bytes
 * Press enter to contiue
 *
 * PID [31764] Requesting 8192 bytes
 * Press enter to contiue
 *
 * PID [31764] Requesting 12288 bytes
 * Press enter to contiue
 *
 * PID [31764] Requesting 16384 bytes
 * Press enter to contiue
 *
 * PID [31764] Requesting 20480 bytes
 * Press enter to contiue
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main() {
	int size = 5;
	void *mem[size];
	pid_t pid = getpid();

	for (int i = 0; i < size; i++) {
		int req_mem = getpagesize() * (i + 1);
		printf("PID [%d] Requesting %d bytes\n", pid, req_mem);

		char *buf = malloc(req_mem);
		if (!buf) {
			printf("Failed to allocate the memory");
			exit(-1);
		}

		mem[i] = buf;
		// Making the memory dirty
		buf[0] = 'D';

		printf("Press enter to contiue\n");
		getchar();
	}

	for (int i = 0; i < size; i++) {
		printf("PID [%d] Releasing memory\n", pid);
		free(mem[i]);

		printf("Press enter to contiue\n");
		getchar();
	}
}

