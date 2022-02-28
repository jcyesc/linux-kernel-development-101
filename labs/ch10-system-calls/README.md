
# System calls


## How to add a system call

The steps to add a system call are in:

- https://github.com/jcyesc/linux-kernel-internals-LFD420/commit/a94b4819ee08fa1b97e5ae00a8d2e2bb62c54aa1
- https://github.com/jcyesc/linux-kernel-internals-LFD420/commit/c8e9a1e2468669ea449762db03e8d3f4856f1848

> For aarch64 is not necessaary to modify this file:
> `linux/arch/arm/tools/syscall.tbl`


## How execute a system call in user space

Once that Linux has booted, compile the following code.


```c
#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

int main()
{
        int sys_custom_number = 441;
        long int result = syscall(sys_custom_number);
        printf("System call sys_custom() returned %ld\n", result);
        sys_custom_number = 442;
        result = syscall(sys_custom_number);
        printf("System call sys_custom_print() returned %ld\n", result);

        return 0;
}
```

For more information check https://medium.com/anubhav-shrimal/adding-a-hello-world-system-call-to-linux-kernel-dad32875872
