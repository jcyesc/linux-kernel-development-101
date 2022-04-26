/* **************** LFD420:5.14 s_22/lab_wastemem.c **************** */
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
 * This program will request memory without releasing it till the Out Of Memory
 * (OOM) killer tries to kill the process. You can execute the example with:
 *
 *    $ gcc lab_wastemem.c
 *    $ ./a.out
 *    Total System Memory in MB = 7351 MB
 *    Using somewhat less: 6615 MB
 *
 *    ...
 *    3952    3968    3984    4000    4016    4032    4048    4064    4080
 *    Killed
 *
 *    $ dmesg
 *    [70805.560041] Out of memory: Kill process 10498 (a.out) score 700 or sacrifice child
 *    [70805.560045] Killed process 10498 (a.out) total-vm:6002360kB, anon-rss:5997804kB, file-rss:4kB, shmem-rss:0kB
 *    [70805.705400] oom_reaper: reaped process 10498 (a.out), now anon-rss:0kB, file-rss:0kB, shmem-rss:0kB
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <signal.h>

#define MB (1024*1024)
#define BS 16			/* will allocate BS*MB at each step */
#define CHUNK (MB*BS)
#define QUIT_TIME_IN_SEC 20

void quit_on_timeout(int sig)
{
	printf("\n\nTime expired, quitting\n");
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
	struct sysinfo si;
	int j, m;
	char *c;

	/* get total memory on the system */
	sysinfo(&si);
	m = si.totalram / MB;
	printf("Total System Memory in MB = %d MB\n", m);
	m = (9 * m) / 10;	/* drop 10 percent */
	printf("Using somewhat less: %d MB\n", m);

	if (argc == 2) {
		m = atoi(argv[1]);
		printf("Choosing instead mem = %d MB\n", m);
	}

	signal(SIGALRM, quit_on_timeout);
	printf("Will quit in QUIT_TIME_IN_SEC seconds if no normal termination\n");
	alarm(QUIT_TIME_IN_SEC);

	for (j = 0; j <= m; j += BS) {
		/* yes we know this is a memory leak, no free,
		 * that's the idea!
		 */
		c = malloc(CHUNK);
		/* just fill the block with j over and over */
		memset(c, j, CHUNK);
		printf("%8dMB", j);
		fflush(stdout);
	}
	printf("\n\n    Sleeping for 5 seconds\n");
	sleep(5);
	printf("\n\n    Quitting and releasing memory\n");
	exit(EXIT_SUCCESS);
}

