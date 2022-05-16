/* **************** LFD420:5.14 s_19/lab_hugepage_mm.c **************** */
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
 * Using Huge Pages with Memory Mapping
 *
 * Write a simple program that opens a file that it creates, and uses
 * only memory mapped access to it; i.e., do not use normal reads and
 * writes.
 *
 * The program should stuff values into the memory locations in the
 * mapped area, and then read them back to make sure they are valid.
 *
 * There is nothing in the program itself that mandates huge pages;
 * you will make it do so by the following two steps:
 *
 *   Create large pages by modifying the value of
 *   /proc/sys/vm/nr_hugepages.  You may have to try this a few times
 *   if your memory is fragmented, to get up to your full request.
 *
 *      $ sudo sh -c 'echo 10 > /proc/sys/vm/nr_hugepages'
 *
 *
 *   Mount the hugetlbfs filesystem somewhere, and make sure the file
 *   you open resides on that filesystem.
 *
 *      $ mkdir -p /mnt/huge
 *      $ sudo mount -t hugetlbfs none /mnt/huge -o size=100M
 *
 * While mounting, specify a size for the filesystem.  Note that
 * unless you change permissions on the mount point, only superusers
 * will be able to create files there.
 *
 * Monitor the number of huge pages being used while the program runs
 * to make sure you are actually accessing them.  You'll probably need
 * to put a short sleep in the code to give time enough to see it, and
 * you can do something like:
 *
 *      $ watch -d -n 1 cat /proc/meminfo
 *
 * in a terminal window to keep your eye on it.
 *
 * Note: We haven't discussed memory mapping, but you really
 * should know how to write a simple user program that memory maps a
 * file anyway.
 *
 * Compile and run the program with:
 *
 *    $ gcc lab_hugepage_mm.c
 *    $ sudo ./a.out
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>

#define DEATH(mess) { perror(mess); exit(errno); }

#define MB (1024 * 1024)

int main(int argc, char *argv[])
{
	char *addr;
	/* The file has to be created under the hugetlbfs file system that was
	 * mounted, otherwise we would get Kernel Bus errors.
	 */
	char *filename = "/mnt/huge/hugemmtestfile";
	int fd;
	unsigned long j;
	unsigned long size = 4 * MB;

	if (argc > 1)
		filename = argv[1];

	if (argc > 2)
		size = atoi(argv[2]) * MB;

	printf("Opening file %s \n", filename);
	fd = open(filename, O_CREAT | O_RDWR, 0755);
	if (fd < 0)
		DEATH("Failed to open the file");

	addr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED) {
		unlink(filename);
		DEATH("Failed to mmap the file");
	}

	printf("\nfile=%s, open on fd=%d, mmaped to address= %p\n",
	       filename, fd, addr);

	/* can close and unlink now */
	printf("Closing and unlinking the file %s\n", filename);
	close(fd);
	unlink(filename);

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
			munmap(addr, size);
			exit(EXIT_FAILURE);
		}

	printf("\nOK, we checked the values, and they were OK up to %ld\n", j);

	/* good hygiene, would munmap on closing anyway */
	munmap(addr, size);
	exit(EXIT_SUCCESS);
}
