
/*
 * User space add that shows the usage of the device node /dev/samcdev101
 *
 * $ arm-linux-gnueabi-gcc app_for_cdev.c  -o exec
 * $ scp exec pi@192.168.2.2:~
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char *dev_node = "/dev/samcdev101";

void write_msg(char msg[], int length) {
	// fd1 will be used to write a message.
	int fd1;
	int ret;

	fd1 = open(dev_node, O_RDWR);
	if (fd1 < 0) {
		perror("Error opening device node");
	}

	printf("%s is open. File descriptor [%d] \n", dev_node, fd1);
	ret = write(fd1, msg, length);
	printf ("Return code from write %d, msg = %s\n", ret, msg);

	close(fd1);
}

void read_msg(char msg[], int length) {
	// fd2 will be used to read it.
	int fd2;
	int ret;

	fd2 = open(dev_node, O_RDWR);
	if (fd2 < 0) {
		perror("Error opening device node");
	}

	printf("%s is open. File descriptor [%d] \n", dev_node, fd2);

	// Clean the buffer.
	memset(msg, 0, length);

	ret = read(fd2, msg, length);
	printf("Return code from read %d, msg = %s\n", ret, msg);

	close(fd2);
}

int main(int argc, char **arg)
{
	char msg[] = "This is a welcome message";
	int length = sizeof(msg);

	write_msg(msg, length);
	read_msg(msg, length);

	return 0;
}

