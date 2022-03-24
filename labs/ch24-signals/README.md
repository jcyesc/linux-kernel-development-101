
# Chapter 24 - Signals


- What are signals
- Available signals
- System calls for signals
- Sigaction
- Signals and Threads
- How the kernel install signal handlers
- How the kernel sends signals
- How the kernel invokes signal handlers
- Real time signals


## Signal

A `signal` is a message sent to a task (process, thread or group) to perform
something.


## Available signals

The list of available tasks are in:

- https://elixir.bootlin.com/linux/v5.16.5/source/include/uapi/asm-generic/signal.h


```c
#define SIGHUP		 1
#define SIGINT		 2
#define SIGQUIT		 3
#define SIGILL		 4
#define SIGTRAP		 5
#define SIGABRT		 6
#define SIGIOT		 6
#define SIGBUS		 7
#define SIGFPE		 8
#define SIGKILL		 9
#define SIGUSR1		10
#define SIGSEGV		11
#define SIGUSR2		12
#define SIGPIPE		13
#define SIGALRM		14
#define SIGTERM		15
#define SIGSTKFLT	16
#define SIGCHLD		17
#define SIGCONT		18
#define SIGSTOP		19
#define SIGTSTP		20
#define SIGTTIN		21
#define SIGTTOU		22
#define SIGURG		23
#define SIGXCPU		24
#define SIGXFSZ		25
#define SIGVTALRM	26
#define SIGPROF		27
#define SIGWINCH	28
#define SIGIO		29
#define SIGPOLL		SIGIO
/*
#define SIGLOST		29
*/
#define SIGPWR		30
#define SIGSYS		31
#define	SIGUNUSED	31

```

The different signals are represented in this data structure:


```c
#define _NSIG		64
#define _NSIG_BPW	__BITS_PER_LONG
#define _NSIG_WORDS	(_NSIG / _NSIG_BPW)

typedef struct {
   unsigned long sig[_NSIG_WORDS];
} sigset_t;
```

The size of `sigset_t` is always `64 bits = sizeof (sigset_t)`


To see the available signals in Linux, you can run:

```shell
$ kill -l
 1) SIGHUP	 2) SIGINT	 3) SIGQUIT	 4) SIGILL	 5) SIGTRAP
 6) SIGABRT	 7) SIGBUS	 8) SIGFPE	 9) SIGKILL	10) SIGUSR1
11) SIGSEGV	12) SIGUSR2	13) SIGPIPE	14) SIGALRM	15) SIGTERM
16) SIGSTKFLT	17) SIGCHLD	18) SIGCONT	19) SIGSTOP	20) SIGTSTP
21) SIGTTIN	22) SIGTTOU	23) SIGURG	24) SIGXCPU	25) SIGXFSZ
26) SIGVTALRM	27) SIGPROF	28) SIGWINCH	29) SIGIO	30) SIGPWR
31) SIGSYS	34) SIGRTMIN	35) SIGRTMIN+1	36) SIGRTMIN+2	37) SIGRTMIN+3
38) SIGRTMIN+4	39) SIGRTMIN+5	40) SIGRTMIN+6	41) SIGRTMIN+7	42) SIGRTMIN+8
43) SIGRTMIN+9	44) SIGRTMIN+10	45) SIGRTMIN+11	46) SIGRTMIN+12	47) SIGRTMIN+13
48) SIGRTMIN+14	49) SIGRTMIN+15	50) SIGRTMAX-14	51) SIGRTMAX-13	52) SIGRTMAX-12
53) SIGRTMAX-11	54) SIGRTMAX-10	55) SIGRTMAX-9	56) SIGRTMAX-8	57) SIGRTMAX-7
58) SIGRTMAX-6	59) SIGRTMAX-5	60) SIGRTMAX-4	61) SIGRTMAX-3	62) SIGRTMAX-2
63) SIGRTMAX-1	64) SIGRTMAX
```

## System calls for signals

```c
#include <sys/types.h>
#include <signal.h>

int kill (pid_t pid, int sig);
int raise (int sig);
void (*signal (int signum, void (*sighandler)(int)))(int);
int sigaction (int signum, const struct sigaction *act,
struct sigaction *oldact);

int sigprocmask (int how, const sigset_t *set, sigset_t *oldset);
int sigpending (sigset_t *set);
int sigsuspend (const sigset_t *mask);
```

and

```c
#include <signal.h>
int sigemptyset (sigset_t *set);
int sigfillset (sigset_t *set);
int sigaddset (sigset_t *set, int signum);
int sigdelset (sigset_t *set, int signum);
int sigismember (const sigset_t *set, int signum);
```

## Sigaction

sigaction is a data structure and function that replaces `signal()`. It provides
more functionality than `signal()`.

## Singals and threads

The threads shared the same signal handlers, however, each thread can configure
the signals that will listen. To do that, it will use the functions:

```c
#include <signal.h>

int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset);
int sigwait(const sigset_t *set, int *sig);
```

## How the kernel install signal handlers

Inside `struct task` there are several fields dedicated to signals. The
`struct sighand_struct` data structure contains the handlers for the different
signals and it is this field that has to be updated.


```c
struct sighand_struct {
    atomic_t count;
    struct k_sigaction action[_NSIG];
    spinlock_t siglock;
    wait_queue_head_t signalfd_wqh;
};
```

## How the kernel sends signals

The essential part of sending a signal to a process is merely setting a bit
in the pending signal mask located in struct task.


## How the kernel invokes signal handlers

The kernel invokes the signal handler by examining the pending signal mask. If
several bits are set in the mask, the handlers for these signals will be invoked
before the process can resume.


## Real time signals

`Real time` signals differ from ordinary signals in that they can be queue up.
For example, if more than one signal of a given type is received, the signal
handler can be called multiple times.

