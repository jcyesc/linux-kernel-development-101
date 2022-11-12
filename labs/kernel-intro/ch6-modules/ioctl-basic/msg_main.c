/**
 * This program sets messages using the ioctl() system call on the device
 * node /dev/messagedev.
 *
 * To compile and execute:
 *
 *    arm-linux-gnueabi-gcc -o msg_main.bin -I./ msg_main.c
 *    scp msg_main.bin pi@192.168.2.2:~
 *
 *    pi@raspberrypi:~ $ ./msg_main.bin
 *    open() returns 3
 *    LOG: print message in kernel
 *    GET: The message is [Default message :)]
 *    SET: message [Welcome to Userpace!!!!]
 *    GET: The message is [Welcome to Userpace!!!!]
 *    LOG: print message in kernel
 *    pi@raspberrypi:~ $ ./msg_main.bin
 *    open() returns 3
 *    LOG: print message in kernel
 *    GET: The message is [Welcome to Userpace!!!!]
 *    SET: message [Welcome to Userpace!!!!]
 *    GET: The message is [Welcome to Userpace!!!!]
 *    LOG: print message in kernel
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "msg_lib.h"

void set_msg(const int fd) {
	struct content contnt = {
		.msg = "Welcome to Userpace!!!!",
	};

	int ret = ioctl(fd, MSG_SET_IOC, &contnt);
	if (ret) {
		printf("Error [%d] while setting the msg\n", ret);
		return;
	}

	printf("SET: message [%s]\n", contnt.msg);
}

void get_msg(const int fd) {
	struct content contnt = {
		.msg = "No updated yet",
	};

	int ret = ioctl(fd, MSG_GET_IOC, &contnt);
	if (ret) {
		printf("Error [%d] while getting the msg\n", ret);
		return;
	}

	printf("GET: The message is [%s]\n", contnt.msg);
}

void log_msg(const int fd) {
	int ret = ioctl(fd, MSG_PRINT_IOC);
	if (ret) {
		printf("Error [%d] while logging the msg\n", ret);
		return;
	}

	printf("LOG: print message in kernel\n");
}

int main(int argc, char *argv[]) {
	char *dev_name = "/dev/messagedev";

	// Device nodes are opened as O_RDONLY.
	const int fd = open(dev_name, O_RDONLY);
	printf("open() returns fd: %d\n", fd);
	if (fd < 0) {
		printf("There was a problem opening %s, error %d\n", dev_name, fd);
		printf("Try: sudo chmod 755 %s\n", dev_name);
		return fd;
	}

	log_msg(fd);
	get_msg(fd);
	set_msg(fd);
	get_msg(fd);
	log_msg(fd);

	close(fd);
	exit(EXIT_SUCCESS);
}

