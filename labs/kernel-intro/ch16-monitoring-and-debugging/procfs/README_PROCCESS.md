
# /proc pseudo file system (processes)

The /proc pseudo file system contains information about the processes, such as

- memory usage (vm and physical memory)
- file descriptors
- stats
- signals
- status

## Analizying a process

The program `mem_request.c` is used to see how the memory allocation is
reflected in the /proc/pid/smaps file.

To start, run an execute the program:

```shell
$ gcc -o mem_request mem_request.c
$ ./mem_request
PID [1765] Requesting 4096 bytes
Press enter to contiue

PID [1765] Requesting 8192 bytes
Press enter to contiue

PID [1765] Requesting 12288 bytes
Press enter to contiue

PID [1765] Requesting 16384 bytes
Press enter to contiue
```

Under `/proc/1765` you can see the files and directories below:

```shell
$ ls /proc/1765/
attr/            exe              mounts           projid_map       status
autogroup        fd/              mountstats       root/            syscall
auxv             fdinfo/          net/             sched            task/
cgroup           gid_map          ns/              schedstat        timers
clear_refs       io               numa_maps        sessionid        timerslack_ns
cmdline          limits           oom_adj          setgroups        uid_map
comm             loginuid         oom_score        smaps            wchan
coredump_filter  map_files/       oom_score_adj    smaps_rollup
cpuset           maps             pagemap          stack
cwd/             mem              patch_state      stat
environ          mountinfo        personality      statm

$ ls -lh /proc/1765
total 0
dr-xr-xr-x 2 parallels parallels 0 Feb 17 17:12 attr
-rw-r--r-- 1 parallels parallels 0 Feb 17 17:12 autogroup
-r-------- 1 parallels parallels 0 Feb 17 17:12 auxv
-r--r--r-- 1 parallels parallels 0 Feb 17 17:12 cgroup
--w------- 1 parallels parallels 0 Feb 17 17:12 clear_refs
-r--r--r-- 1 parallels parallels 0 Feb 17 17:12 cmdline
-rw-r--r-- 1 parallels parallels 0 Feb 17 17:12 comm
-rw-r--r-- 1 parallels parallels 0 Feb 17 17:12 coredump_filter
-r--r--r-- 1 parallels parallels 0 Feb 17 17:12 cpuset
lrwxrwxrwx 1 parallels parallels 0 Feb 17 17:12 cwd -> /home/parallels/procfs
-r-------- 1 parallels parallels 0 Feb 17 17:12 environ
lrwxrwxrwx 1 parallels parallels 0 Feb 17 17:12 exe -> /home/parallels/procfs/mem_request
dr-x------ 2 parallels parallels 0 Feb 17 17:12 fd
dr-x------ 2 parallels parallels 0 Feb 17 17:12 fdinfo
-rw-r--r-- 1 parallels parallels 0 Feb 17 17:12 gid_map
-r-------- 1 parallels parallels 0 Feb 17 17:12 io
-r--r--r-- 1 parallels parallels 0 Feb 17 17:12 limits
-rw-r--r-- 1 parallels parallels 0 Feb 17 17:12 loginuid
dr-x------ 2 parallels parallels 0 Feb 17 17:12 map_files
-r--r--r-- 1 parallels parallels 0 Feb 17 17:12 maps
-rw------- 1 parallels parallels 0 Feb 17 17:12 mem
-r--r--r-- 1 parallels parallels 0 Feb 17 17:12 mountinfo
-r--r--r-- 1 parallels parallels 0 Feb 17 17:12 mounts
-r-------- 1 parallels parallels 0 Feb 17 17:12 mountstats
dr-xr-xr-x 5 parallels parallels 0 Feb 17 17:12 net
dr-x--x--x 2 parallels parallels 0 Feb 17 17:12 ns
-r--r--r-- 1 parallels parallels 0 Feb 17 17:12 numa_maps
-rw-r--r-- 1 parallels parallels 0 Feb 17 17:12 oom_adj
-r--r--r-- 1 parallels parallels 0 Feb 17 17:12 oom_score
-rw-r--r-- 1 parallels parallels 0 Feb 17 17:12 oom_score_adj
-r-------- 1 parallels parallels 0 Feb 17 17:12 pagemap
-r-------- 1 parallels parallels 0 Feb 17 17:12 patch_state
-r-------- 1 parallels parallels 0 Feb 17 17:12 personality
-rw-r--r-- 1 parallels parallels 0 Feb 17 17:12 projid_map
lrwxrwxrwx 1 parallels parallels 0 Feb 17 17:12 root -> /
-rw-r--r-- 1 parallels parallels 0 Feb 17 17:12 sched
-r--r--r-- 1 parallels parallels 0 Feb 17 17:12 schedstat
-r--r--r-- 1 parallels parallels 0 Feb 17 17:12 sessionid
-rw-r--r-- 1 parallels parallels 0 Feb 17 17:12 setgroups
-r--r--r-- 1 parallels parallels 0 Feb 17 17:12 smaps
-r--r--r-- 1 parallels parallels 0 Feb 17 17:12 smaps_rollup
-r-------- 1 parallels parallels 0 Feb 17 17:12 stack
-r--r--r-- 1 parallels parallels 0 Feb 17 17:12 stat
-r--r--r-- 1 parallels parallels 0 Feb 17 17:12 statm
-r--r--r-- 1 parallels parallels 0 Feb 17 17:12 status
-r-------- 1 parallels parallels 0 Feb 17 17:12 syscall
dr-xr-xr-x 3 parallels parallels 0 Feb 17 17:12 task
-r--r--r-- 1 parallels parallels 0 Feb 17 17:12 timers
-rw-rw-rw- 1 parallels parallels 0 Feb 17 17:12 timerslack_ns
-rw-r--r-- 1 parallels parallels 0 Feb 17 17:12 uid_map
-r--r--r-- 1 parallels parallels 0 Feb 17 17:12 wchan
```

### /proc/<pid>/cmdline


```shell
$ cat /proc/1765/cmdline
./mem_request$
```

### /proc/<pid>/limits


```shell
$ cat  /proc/1765/limits
Limit                     Soft Limit           Hard Limit           Units
Max cpu time              unlimited            unlimited            seconds
Max file size             unlimited            unlimited            bytes
Max data size             unlimited            unlimited            bytes
Max stack size            8388608              unlimited            bytes
Max core file size        0                    unlimited            bytes
Max resident set          unlimited            unlimited            bytes
Max processes             31644                31644                processes
Max open files            1024                 1048576              files
Max locked memory         65536                65536                bytes
Max address space         unlimited            unlimited            bytes
Max file locks            unlimited            unlimited            locks
Max pending signals       31644                31644                signals
Max msgqueue size         819200               819200               bytes
Max nice priority         0                    0
Max realtime priority     0                    0
Max realtime timeout      unlimited            unlimited            us
```

### /proc/<pid>/task

To check the threads created by the process, we can use:

```shell
$ ls -l  /proc/1765/task/
total 0
dr-xr-xr-x 7 parallels parallels 0 Feb 17 18:13 1765
```

### /proc/<pid>/environ

To find out the environment variables available to the process, we can execute:

```shell
$ cat  /proc/1765/environ
QT_IM_MODULE=ibusQT_QPA_PLATFORMTHEME=appmenu-qt5XDG_SESSION_TYPE=x11
PWD=/home/parallels/procfs
JOB=unity-settings-daemon
XMODIFIERS=@im=ibus
GNOME_KEYRING_PID=LANG=en_US.UTF-8GDM_LANG=en_USMANDATORY_
PATH=/usr/share/gconf/text.path
```

### File descriptors (fd)

Every process has 3 file descriptors:

0 * Standard input
1 - Standard output
2 - Standard error

These can be found in:

```shell
$ ls -l  /proc/1765/fd
total 0
lrwx------ 1 parallels parallels 64 Feb 17 17:44 0 -> /dev/pts/18
lrwx------ 1 parallels parallels 64 Feb 17 17:44 1 -> /dev/pts/18
lrwx------ 1 parallels parallels 64 Feb 17 17:44 2 -> /dev/pts/18

$ ls -l  /proc/8302/fd
total 0
lrwx------ 1 parallels parallels 64 Feb 17 17:45 0 -> /dev/pts/21
lrwx------ 1 parallels parallels 64 Feb 17 17:45 1 -> /dev/pts/21
lrwx------ 1 parallels parallels 64 Feb 17 17:45 2 -> /dev/pts/21

$ lsof /dev/pts/18
COMMAND    PID      USER   FD   TYPE DEVICE SIZE/OFF NODE NAME
mem_reque 1765 parallels    0u   CHR 136,18      0t0   21 /dev/pts/18
mem_reque 1765 parallels    1u   CHR 136,18      0t0   21 /dev/pts/18
mem_reque 1765 parallels    2u   CHR 136,18      0t0   21 /dev/pts/18
bash      4161 parallels    0u   CHR 136,18      0t0   21 /dev/pts/18
bash      4161 parallels    1u   CHR 136,18      0t0   21 /dev/pts/18
bash      4161 parallels    2u   CHR 136,18      0t0   21 /dev/pts/18
bash      4161 parallels  255u   CHR 136,18      0t0   21 /dev/pts/18
```

To send data to the standard output for the process `1765`, we can execute:

```shell
$ echo "hello" >  /proc/1765/fd/1
```

In the console where the process is running, we will see:

```shell
$ ./mem_request
PID [1765] Requesting 4096 bytes
Press enter to contiue

PID [1765] Requesting 8192 bytes
Press enter to contiue

PID [1765] Requesting 12288 bytes
Press enter to contiue

PID [1765] Requesting 16384 bytes
Press enter to contiue

PID [1765] Requesting 20480 bytes
Press enter to contiue
hello
hello
hello
```

### /proc/pid/fdinfo

To find out the read/write position of the files open by the process, we can
run:

```shell
$ cat  /proc/1765/fdinfo/1
pos:	0
flags:	0102002
mnt_id:	25
$ cat  /proc/1765/fdinfo/0
pos:	0
flags:	0102002
mnt_id:	25
$ cat  /proc/1765/fdinfo/2
pos:	0
flags:	0102002
mnt_id:	25
```

### Stop/Cont a proccess

To stop and resume a process, we can run in another terminal:

```shell
# Stop a process
$ kill -STOP 1765

# Resume a process
$ kill -CONT 1765
```

In order to bring the process to foreground, in the terminal where the process
was running, execute:


```shell
$ jobs
[1]+  Stopped                 ./mem_request
$ fg ./mem_request
./mem_request

PID [1765] Releasing memory
Press enter to contiue
```

### Thread states and contect switches

The different states a thread can be found in `/include/linux/sched.h`.

- TASK_RUNNING
- TASK_INTERRUPTIBLE
- TASK_UNINTERRUPTIBLE
- TASK_STOPPED
- TASK_TRACED
- EXIT_ZOMBIE
- EXIT_DEAD

A thread will continue running till is `preempted` or go to `sleep/wait`.


- `preemption`: occurs when due to an external interrupt caused by the timer
or a higher priority process, the current proccess is stopped temporaly. When
a new process is selected to run, a `context switch` happens. When the running
process decided to sleep/wait, we say that it is a `voluntary context switch`.
If the process was preempted by other process, it is a `nonvoluntary context switch`.

- `sleep/wait`: occurs when the thread does have nothing to do at the present
moment.


A high `voluntary context switch` count usually implies heavy I/O, whereas a
hign-non `voluntary context switch` count could mean the thread is CPU bound.


## cgroups

cgroups is a Linux facility that allows to group threads that will have same
cpu affinity, memory usage, block io config, etc.

The table below shows the control group types:


| Controller   |   Mount Point |  Policy |
|:----------|:-------------|:------|
| block i/O | /dev/blkio | Block device I/O limits |
| cpu  | /dev/cpuset | Thread/core affinity |
| cpuset | /acc | CPU accounting statistics |
| cpuctl | /dev/cpuctl | CPU timing shares  |
| freezer | /dev/freezer | Group freeze/thaw |
| memory | /dev/memcg | Memory utilization |
| sched tuning | /dev/stune | Task prioritization |

