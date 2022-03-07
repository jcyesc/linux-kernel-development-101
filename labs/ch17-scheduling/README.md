
# Chapter 17 - Scheduling

The main job of the scheduler consists in assigning a time slice to the processes
that are running in the system. The time slice assigment could be based on
size of the process, if it is interactive or not, the priority of the process, etc.


## Scheduler Policies

The scheduler policies defined in the Linux kernel 5.16.12 are:

- https://elixir.bootlin.com/linux/latest/source/include/uapi/linux/sched.h#L120

```c
/*
 * Scheduling policies
 */
#define SCHED_NORMAL		0
#define SCHED_FIFO		1
#define SCHED_RR		2
#define SCHED_BATCH		3
/* SCHED_ISO: reserved but not implemented yet */
#define SCHED_IDLE		5
#define SCHED_DEADLINE		6
```

- SCHED_NORMAL; is the default policy and it means that all the tasks will compete
for a time slice.
- SCHED_FIFO: if a task has this policy, the tasks will be executed in First In
First Out order.
- SCHED_RR:  Under the round robin policy CPU time is shared, rotated, with other
real time priority tasks with the same priority.
- SCHED_DEADLINE: The Deadline scheduler is based on completing a task before a
particular time.


## nice()

In the userspace, there is a function that allows to change the priority of the
process: `nice()`. The nice function receives a number between -20 and 19. The
bigger the number, the nicest it is, the smaller the number, the less nice a proccess
is.


## The 2.4 schedule() Function

The scheduler in the 2.4 Linux kernel version had to iterate through all the
process availables, see

- https://elixir.bootlin.com/linux/2.4.27/source/kernel/sched.c#L606

This scheduler also needed to make sure that we don't execute the task in more
than one CPU.

## O(1) Scheduler

The `O(1)` scheduler was introduced in the 2.5.2 Linux kernel version. This
scheduler takes its name from the fact that it finds the next task to run in `O(1)`
time. The features of this scheduler are:

- Has two queues per CPU: `active` and `expired`.
- Every queue has inside a priority-sorted array of queues.
- When all the tasks in the `active` queue have been executed, the `expired`
queue becomes the `active` queue. This gives the change to tasks with small
priority to run.
- There is an spinlock per queue per CPU.
- Real time tasks are handled differently and shared one global queue.


## Priority inversion

Suppose we have three tasks: A, B and C. A has the highest priority and C has the
lowest priority. B's priority is in between A and C. Now, we have this situation:

- C grabs a lock x.
- A is waiting to grab lock x.
- Because A is waiting, B starts running, even if it has lower priority.

This situation is called `priority inversion`.

This situation can be avoided by implementing `priority inheritance`. This means
that when a lower priority task has a lock on a resource that a highest priority
task is waiting on, the lower priority task will inherit temporaly the priority
of the highest priority task. This solution is not perfect and could allow
a lower priority task to abuse this mechanism to hold the lock longer than required.


## The CFS Scheduler

The 2.6.23 introduced the Completely Fair Scheduler (CFS) which uses a
`red black tree` instead of a `priority queue`.

The elements in the tree are sorted based on the notion on how much time the task
is entitled to, based on some timing statistics.


## The Deadline Scheduler

The 3.14 kernel introduced the `deadline scheduler`. This scheduler allows to
specify when the task has to run and for how long.


## The Real Time Scheduler

Behind the scenes, this scheduler uses the O(1) scheduler. It finds the highest
priority queue that is not empty and takes the next from it.

