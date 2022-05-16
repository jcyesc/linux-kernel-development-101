
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#define DEATH(mess) { perror(mess); exit(errno); }

/*
 * This program shows how to lock memory and prevent swapping out.
 */

int main(int argc, char *argv[]) {
	char *buf;
	int length;
	int np = 4;
	int page_size;

	if (argc > 1)
		np = atoi(argv[1]);

	page_size = getpagesize();
	length = np * page_size;
	printf ("The page size is %d bytes!\n", page_size);

	buf = malloc(length);
	if (buf == NULL)
		DEATH("mallocing");

	if (mlock(buf, length))
		DEATH("mlock:");
	printf("Succeeding in locking memory, %d pages!\n", np);

	/*
	 * Locking the memory prevents the process memory from being swapped out.
	 */

	if (munlock(buf, length))
		DEATH("munlock:");
	printf("Succeding in unlocking memory!\n");
	exit(EXIT_SUCCESS);
}
