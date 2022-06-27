
# Timing Measurements

## Timer hardware components

- `System timer`: Generates time events periodically. This time events are handled
by the `timer interrupt`.
- `Real Time Clock (RTC)`: Provides a nonvolatile device for storing the system time.
The RTC continues to keep track of time even when the system is off by way of a
small battery typically included on the system board. On the PC architecture, the
RTC and the CMOS are integrated, and a single battery keeps the RTC running and the
BIOS settings preserved.

On boot, the kernel reads the RTC and uses it to initialize the wall time, which is
stored in the `xtime` variable.

## Kernel notion of time

The system timer harware generates a time event at a programmed frequency called
`tick rate`. When the system timer goes off, it issues an interrupt that the kernel
handles via a special interrupt handler. The perioid is called `tick` and is equal
to `1/(tick rate) seconds`.

The main tasks of the timer interrupt are:

- Updating the system uptime.
- Updating the time of day.
- On an SMP system, ensuring that the scheduler runqueues are balanced and, if not,
balancing them.
- Running any dynamic timers that have expired.
- Updating resource usage and processor time statistics.


## The Tick Rate: `HZ`

The frequency of the system timer (the tick rate0 is programmed on system boot based
on a static preprocessor define `HZ`.

The kernel defines the value in `<asm/param.h>`.

## Jiffies

The global variable `jiffies` holds the number of ticks that have occurred since
the system booted. On boot, the kernel initializes the variable to zero, and it is
incremented by one during each timer interrupt. Thus, because there are `HZ` timer
interrupts in a second, there are `HZ` jiffies in a second. The system uptime is
therefore `jiffies/HZ` seconds.

```
jiffies = (seconds * HZ)
seconds = (jiffies / HZ)
```

The jiffies variable is defined in `<linux/jiffies.h>`

```c
extern unsigned long volatile jiffies;
extern u64 jiffies_64;
```
The kernel linker script overlays the `jiffies` variable over the start of the
`jiffies_64` variable.

```c
jiffies = jiffies_64l
```

Thus, jiffies is the lover 32 bits of the full 64-bit jiffies_64 variable.


## BogoMIPS

The `BogoMIPS` value is the number of busy loop iterations the processor can
perform in a given period. In effect, BogoMIPS are a measurement of how fast a
processor can do nothing. This value is stored in the `loops_per_jiffy` variable
and is readable from `/proc/cpuinfo`.

The kernel computes `loops_per_jiffy` on boot via `calibrate_delay()` in
`init/main.c`.

