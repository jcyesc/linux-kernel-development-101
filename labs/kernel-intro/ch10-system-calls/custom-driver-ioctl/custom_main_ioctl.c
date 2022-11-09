/**
 * This program uses ioctl() to invoke a custom device driver functionality.
 * To do that, it first opens a device node file and then calls ioctl().
 *
 * This files has to be compiled in the target device, in this case the RPi 3
 * or crosscompile using arm32 option.
 *
 * To compile and run:
 *
 *    arm-linux-gnueabi-gcc custom_main_ioctl.c -o custom_main_ioctl.bin
 *    scp custom_main_ioctl.bin pi@192.168.2.2:~
 *
 *    pi$ sudo ./custom_main_ioctl.bin
 */


#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int fd, i;
	char *dev_name = "/dev/customdev";

	// Device nodes are opened as O_RDONLY.
	fd = open(dev_name, O_RDONLY);
	printf("open() returns %d\n", fd);
	if (fd < 0) {
		printf("There was a problem opening %s, error %d\n", dev_name, fd);
		printf("Try: sudo chmod 755 %s\n", dev_name);
		return fd;
	}

	for (i = 100; i <= 110; i++) {
		printf("ioctl(%d) = %d\n", i, ioctl(fd, i, 90 + i));
	}

	close(fd);
	exit(EXIT_SUCCESS);
}
