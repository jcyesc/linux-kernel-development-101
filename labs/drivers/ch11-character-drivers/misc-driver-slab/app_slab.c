
/*
 * User space add that shows the usage of the device node /dev/miscslab101
 *
 * $ arm-linux-gnueabi-gcc app_slab.c -o exec
 * $ scp exec pi@192.168.2.2:~
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char *dev_node = "/dev/miscslab101";

void write_msg(int fd, char msg[], int length) {
	int ret;

	printf("%s is open. File descriptor [%d].\n", dev_node, fd);
	ret = write(fd, msg, length);
	printf ("Return code from write [%d]\n", ret);
}

void read_msg(int fd, char msg[], int length) {
	int ret;

	printf("%s is open. File descriptor [%d] \n", dev_node, fd);

	// Clean the buffer.
	memset(msg, 0, length);

	ret = read(fd, msg, length);
	printf("Return code from read [%d], msg = [%s]\n", ret, msg);
}

int main(int argc, char **arg)
{
	char msg1[] = "This is a welcome message";
	int length1 = sizeof(msg1);
	int fd1;
	char msg2[] = "Bienvenidos";
	int length2 = sizeof(msg2);
	int fd2;
	char temp[40];

	fd1 = open(dev_node, O_RDWR);
	if (fd1 < 0) {
		perror("Error opening device node");
	}

	fd2 = open(dev_node, O_RDWR);
	if (fd2 < 0) {
		close(fd1);
		perror("Error opening device node");
	}

	write_msg(fd1, msg1, length1);
	write_msg(fd2, msg2, length2);

	off_t offset = 0;
	lseek(fd1, offset, SEEK_SET);
	lseek(fd2, offset, SEEK_SET);

	read_msg(fd1, temp, length1);
	read_msg(fd2, temp, length2);

	offset = 5;
	lseek(fd1, offset, SEEK_SET);
	lseek(fd2, offset, SEEK_SET);

	read_msg(fd1, temp, length1);
	read_msg(fd2, temp, length2);

	close(fd1);
	close(fd2);

	return 0;
}

