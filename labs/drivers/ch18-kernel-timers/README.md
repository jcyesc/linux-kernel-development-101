
# Chapter 18 - Timers

Timers, sometimes called `dynamic timers` or `kernel timers` - are essential
for managing the flow of time in kernel code. Kernel code ofthen needs to delay
execution of some function until a later time.

## Small delays

The Linux kernel functions implemented to wait for small delays are defined
in `<linux/delay.h>` and `<asm/delay.h>`

```c
void udelay(unsigned long usecs)
void ndelay(unsigned long nsecs)
void mdelay(unsigned long msecs) 
```

## schedule_timeout()

The `schedule_timeout()` is used to delay the execution of a task. This call
puts the task to sleep until at least the specified time has elapsed. There is
no guarantee that the sleep duration will be exactly the specified time, only
that the duration is at least as long as specified.

