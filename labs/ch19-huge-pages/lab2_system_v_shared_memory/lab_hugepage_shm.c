/* **************** LFD420:5.14 s_19/lab_hugepage_shm.c **************** */
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
 * Using Huge Pages with System V Shared Memory
 *
 * Do the same thing with System V shared memory.
 *
 * The instructions are basically the same except you do not have to
 * mount the hugetlbfs filesystem.
 *
 * You may find your program only works when you run it as root. You
 * should be able to fix this by modifying
 * /proc/sys/vm/hugetlb_shm_group to include a group that the user
 * belongs to.
 *
 * To test the program, execute:
 *
 *       $ sudo sh -c 'echo 10 > /proc/sys/vm/nr_hugepages'
 *       $ gcc lab_hugepage_shm.c
 *       $ sudo ./a.out
 *
 * To watch how the pages are allocated, run:
 *
 *      $ watch -d -n 1 cat /proc/meminfo
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MB (1024 * 1024)

int main(int argc, char *argv[])
{
	int shmid, rc = 0;
	char *addr;
	unsigned long j, size = 16 * MB;

	if (argc > 1)
		size = atoi(argv[1]) * MB;

	shmid = shmget(IPC_PRIVATE, size, 0666 | SHM_HUGETLB | IPC_CREAT);
	if (shmid < 0) {
		fprintf(stderr, "Failed in shmget");
		rc = -1;
		goto out;
		exit(EXIT_FAILURE);
	}

	addr = shmat(shmid, (void *)0, 0);
	if (addr == (char *)-1) {
		fprintf(stderr, "Failed to attach to shared memory");
		rc = -1;
		goto out;
	}
	printf("\nshmid %d attached at address= %p\n", shmid, addr);

	/* fill up the region */

	for (j = 0; j < size; j++)
		*(addr + j) = (char)j;
	printf("\nOK, we packed up to %ld with integers\n", size);

	printf("\nPausing 5 seconds so you can see if pages are being used\n");
	sleep(5);

	/* check the values  */
	for (j = 0; j < size; j++)
		if (*(addr + j) != (char)j) {
			printf("Something wrong with value at %ld\n", j);
			rc = -1;
			goto out;
		}

	printf("\nOK, we checked the values, and they were OK up to %ld\n", j);

	if (shmdt((const void *)addr)) {
		fprintf(stderr, "Failure to detach shared memory\n");
		rc = -1;
		goto out;
	}

out:
	shmctl(shmid, IPC_RMID, NULL);
	exit(rc);
}
