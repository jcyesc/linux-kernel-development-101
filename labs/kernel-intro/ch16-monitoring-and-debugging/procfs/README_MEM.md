
# /proc pseudo file system (memory)

## Analizying virtual memory

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

### /proc/pid/smaps

The `smaps` file contains the memory stats for the `mem_request` process
and the shared libraries that it uses (`libc`, etc).


```shell
$ cat /proc/1765/smaps | less
VmFlags: rd wr mr mw me dw ac sd
01110000-01131000 rw-p 00000000 00:00 0                                  [heap]
Size:                132 kB
KernelPageSize:        4 kB
MMUPageSize:           4 kB
Rss:                  20 kB
Pss:                  20 kB
Shared_Clean:          0 kB
Shared_Dirty:          0 kB
Private_Clean:         0 kB
Private_Dirty:        20 kB
Referenced:           20 kB
Anonymous:            20 kB
LazyFree:              0 kB
AnonHugePages:         0 kB
ShmemPmdMapped:        0 kB
Shared_Hugetlb:        0 kB
Private_Hugetlb:       0 kB
Swap:                  0 kB
SwapPss:               0 kB
Locked:                0 kB
```

## /proc/pid/status

The pseudo file `status` shows overall information about the process. For
example the field `VmPTE` contains the amount of memory use by Page Table
Entries (PTF).

```shell
$ cat /proc/1765/status
Name:	mem_request
Umask:	0002
State:	S (sleeping)
Tgid:	1765
Ngid:	0
Pid:	1765
PPid:	4161
TracerPid:	0
Uid:	1000	1000	1000	1000
Gid:	1000	1000	1000	1000
FDSize:	256
Groups:	20 27 1000
NStgid:	1765
NSpid:	1765
NSpgid:	1765
NSsid:	4161
VmPeak:	    4352 kB
VmSize:	    4352 kB
VmLck:	       0 kB
VmPin:	       0 kB
VmHWM:	     652 kB
VmRSS:	     652 kB
RssAnon:	      68 kB
RssFile:	     584 kB
RssShmem:	       0 kB
VmData:	     180 kB
VmStk:	     132 kB
VmExe:	       4 kB
VmLib:	    1952 kB
VmPTE:	      56 kB
VmSwap:	       0 kB
HugetlbPages:	       0 kB
CoreDumping:	0
Threads:	1
SigQ:	0/31644
SigPnd:	0000000000000000
ShdPnd:	0000000000000000
SigBlk:	0000000000000000
SigIgn:	0000000000000000
SigCgt:	0000000000000000
CapInh:	0000000000000000
CapPrm:	0000000000000000
CapEff:	0000000000000000
CapBnd:	0000003fffffffff
CapAmb:	0000000000000000
NoNewPrivs:	0
Seccomp:	0
Speculation_Store_Bypass:	vulnerable
Cpus_allowed:	ffffffff
Cpus_allowed_list:	0-31
Mems_allowed:	00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000000,00000001
Mems_allowed_list:	0
voluntary_ctxt_switches:	6
nonvoluntary_ctxt_switches:	4
```

## Virtual memory classification and lifecycle


### Page lifecycle

The page memory has several states:

| State   |   Meaning |
|:----------|:-------------|
| Free  | Page has not been allocated  |
| Active Clean | Page has been allocated but not modified |
| Active Dirty | Page has been allocated and modified |
| Inactive Clean | Page is allocated but not used (could be freed or reused) |
| Inactive Dirty | Page is allocated and modified and needs to be written back |
| Laundered | Page is selected for writting back |


### Named (mmap) vs Anonymous (malloc)

- `named`: refers to the memory that maps a file (from disk) in memory. The pages
are loaded from a file (via the kernel's page cache), contiguously into the
process virtual memory. The file name serves as the name of the memory region.

- `anonymous`: refers to the memory used by the stack and heap. For example,
the memory allocated by malloc() does not have a name, that is the reason that
is called `anonymous` or`anon`.


### Dirty vs Clean

- `dirty`: a page has been modified by the process(es).
- `clean`: a page has not been modified by the process(es).

The distiction is important because based on the clean status, Linux might
decide to free or write back the pages and then free it.


### Active vs Inactive

Virtual memory pages that are mapped to RAM have an `age`. The kernel and MMU
work together to maintain a reference mechanism, tracking the Least Recent Used
(LRU) pages. When a page is accessed, it is immediately marked as `Active`. It
left unused for more than a given period, the status is changed to `inactive`.

### Private vs Shared

Memory that is accessible only to a process is considered `private`. For example
memory allocated with `malloc`, `new` or mmap() with `MAP_PRIVATE` flag.

Memory could be `shared` between two or more proccess. This could be the result
of mmap with `MAP_SHARED` flag.

The memory could also be `shared` temporaly when `fortk()` is called, so the
child uses the parent memory, however as soon as the child or parent try to write
to the memory a `copy-on-write` is performed.

### Memory statistics

The virtual memory can be classified in 4 disjoint sets:

```c
VmSize = VmRSS + VmFileMapped + VmSwap + VmLazy
```

- `VmRSS`: The Resident Set Size virtual memory refers to the pages that are
backed in the RAM.
- `VmFileMapped`: It refers to the pages that mmap a file.
- `VmSwap` it refers to the `anonymous` memory that has been swapped to disk
or in Android sysstem to `ZRAM` (compressed memory).
- `VmLazy`: it refers to the pages that a process might need but that haven't
been allocated yet by Linux due the process is not using them yet.

To calculate the total memory usage of the system, we use the
`Proportional Set Size` of each process.

The formula is:

```c
             s
PSS = USS +  Σ (shared i) / Pi
           i = 1
```

where

- `s` is the number of share regions
- `shared i` is the sized of shared region i.
- `Pi` is the number of processes sharing the region i.

### /proc/meminfo


```shell
$ cat /proc/meminfo
MemTotal:        7527588 kB
MemFree:         2735820 kB
MemAvailable:    4643880 kB
Buffers:          462068 kB
Cached:          1495536 kB
SwapCached:            0 kB
Active:          3702180 kB
Inactive:         617648 kB
Active(anon):    2363348 kB
Inactive(anon):    84544 kB
Active(file):    1338832 kB
Inactive(file):   533104 kB
Unevictable:          80 kB
Mlocked:              80 kB
SwapTotal:       1046524 kB
SwapFree:        1046524 kB
Dirty:                16 kB
Writeback:           552 kB
AnonPages:       2362316 kB
Mapped:           304760 kB
Shmem:             85672 kB
Slab:             380336 kB
SReclaimable:     342064 kB
SUnreclaim:        38272 kB
KernelStack:        8992 kB
PageTables:        36212 kB
NFS_Unstable:          0 kB
Bounce:                0 kB
WritebackTmp:          0 kB
CommitLimit:     4810316 kB
Committed_AS:    5917880 kB
VmallocTotal:   34359738367 kB
VmallocUsed:           0 kB
VmallocChunk:          0 kB
HardwareCorrupted:     0 kB
AnonHugePages:         0 kB
ShmemHugePages:        0 kB
ShmemPmdMapped:        0 kB
CmaTotal:              0 kB
CmaFree:               0 kB
HugePages_Total:       0
HugePages_Free:        0
HugePages_Rsvd:        0
HugePages_Surp:        0
Hugepagesize:       2048 kB
DirectMap4k:      206768 kB
DirectMap2M:     8181760 kB
```

## Out of Memory

Linux has a mechanism to deal with memory shortage. This mechanism is called
OOM (Out Of Memory). It is triggered when a memory request fails due insufficient
memory. In Linux, this happens very rarely because dirty pages can be swapped to
disk. `It is only when the system is both out of RAM and swap space that OOM
is triggered`.

The OOM is not a thread. It is implemented in the code path of a page fault. The
OOM mechanism looks for a process to be killed. It tried to do the selection
based on these 2 goals: good memory gains and less harm to the system.

All processes are candidates on this `death row`, sorted by their `oom_score`.
This score is available in `/proc/pid/oom_score`.


```shell
$ cat /proc/1765/oom_score
0
$ cat /proc/1765/oom_score_adj
0
```

## Android case of study

### Android: Low memory killer daemon (lmkd)

In Android, the lmkd daemon helps the `ActivityManager` to adjust the killability
score of the processes.

### Proccess and Memory information

Android provides the services that parse `/proc` fs to make the stats more
human readable. Below is a list of the services:

- `cpufino`: service  provides information on system load and individual
processes.
- `processinfo`: service provides the scheduling state of processes.
- `procstats`: service provides the most comprensive set of statistics on
processes in the system.
- `meminfo`: service is one of the few `dumpsys` only services in Android,
designed to provide system and process memory information.

```shell
$ dumpsys meminfo $pid
```

### Peformance hints (Android 12.0)

Android 12.0 introduced a service to provide performance hints to Linux. This
hints indicates the type of load (I/O bound or CPU bound, etc).

### Android 10.0 iorapd

Android 19.0 introduced `iorapd`, an I/O Read Ahead and Pin Daemon. The main
idea is to maximaze the use of Linux's `page cache` by requesting ahead of
time the I/O blocks that will be required. When the block is requested by the
process, it will be accessible instanstly.

### Android pinner service

There are some pages that are hot, which means, they are used frequently. For
this case, we can pin the pages using `mmap()` and `mlock()`.

Android provides the pinner service to pin pages. The pinner service uses JNI
to call `mmap()` and `mlock()`.

```shell
$ dumpsys pinner
```

