
# System calls


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
