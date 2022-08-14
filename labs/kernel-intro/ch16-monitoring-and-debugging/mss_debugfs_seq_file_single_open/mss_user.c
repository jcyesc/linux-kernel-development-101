/*
 * User space program to write to the debugfs file.s
 *
 * To compile and run:
 *
 *    arm-linux-gnueabi-gcc mss_user.c -o mss_user.bin
 *    scp mss_user.bin pi@192.168.2.2:~
 *
 *    pi$ sudo ./mss_user.bin
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main()
{
	int fd;
	char *file_name = "/sys/kernel/debug/mssdbg/mc2/lpddr4-3";
	int ret;
	int size = 10;
	const char *input = "123456";

	printf("Opening %s \n", file_name);

	fd = open(file_name, O_RDWR);
	if(fd < 0) {
		printf("Cannot open file %s \n", file_name);
		return -1;
	}

	ssize_t num = write(fd, input, strlen(input));

	if (num < 0) {
		printf("Error while writing\n");
		close(fd);
		return -1;
	}

	printf("Write was successful.\n");

	close(fd);
}
