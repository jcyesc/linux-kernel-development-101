// SPDX-License-Identifier: GPL-2.0
/*
 * Shared library used by the kernel and userspace.
 */

#ifndef MSG_IOCTL_BASIC_H_
#define MSG_IOCTL_BASIC_H_

#define MSG_IOC_MAGIC '$'

// https://www.kernel.org/doc/html/latest/staging/index.html
// https://elixir.bootlin.com/linux/v5.16.5/source/include/uapi/asm-generic/ioctl.h#L85
// _IO ioctl with no parameters
#define MSG_PRINT_IOC _IO(MSG_IOC_MAGIC, 1)
// _IOW ioctl with write parameters (copy_from_user)
#define MSG_SET_IOC   _IOW(MSG_IOC_MAGIC, 2, struct content)
// _IOR ioctl with read parameters (copy_to_user)
#define MSG_GET_IOC   _IOR(MSG_IOC_MAGIC, 3, struct content)

#define MAX_MSG_SIZE 200

struct content {
	char msg[MAX_MSG_SIZE];
};

#endif // MSG_IOCTL_BASIC_H_
