
# Chapter 22 - Disk Caches and Swapping

In this chapter we discuss

- Caches
- Swapping
- Swap Areas
- Swapping pages in and out
- Swappiness
- Swap cache
- Reverse Mapping
- OOM killer


## Caches

Linux uses different caches such as

- `Hardware caches`: You can see this information in:

```
$ cat /sys/devices/system/cpu/cpu2/cache/index0/size
32KB
```

- `Software caches`

- `Dentry cache` is used to speed up the disk access.

- `Page cache`: Linux uses page caches in order to minimize the disk access due
it is expensive. The `sync` command flushes the modified memory into the block
device. The function `fsync()` flushes the modified memory into the block device.


## Swapping

Swapping is the process of moving data from memory to the disk and viceversa.
This is used to allow more process in memory.

Swapping is a bad thing because it prevents processes from hanging when free
memory is exhausted or it allows to launch a process even when there is not
more physical memory available.

We can find the `swap areas` using:

```shell
$ cat /proc/swaps
Filename				Type		Size	Used	Priority
/dev/sda5                               partition	1046524	0	-2
```

## Swap areas

The swap area is where the pages swapped are going to be stored. This is usually
a location in a file. We can see the swap areas by running:

```shell
$ cat /proc/swaps
Filename				Type		Size	Used	Priority
/dev/sda5                               partition	1046524	0	-2
```

## Swapping pages in and out

- `Swap in` refers to the action of bringing the page from disk and install it in
memory.
- `Swap out` refers to the action of storing the page in memory to disk. The
memory mapped files are not swapped out to disk because they are already stored
in disk.


## Swappiness

If we want to increase the swaps in the system, we can write in the proc file
below:

```shell
$ cat /proc/sys/vm/swappiness
```

The values in `/proc/sys/vm/swappiness` are between 0 and 100 where 0 means
not swaps and 100 means increase the swaps.


## Swap cache

When a page is selected to be swapped out, this page might be shared by several
processes, which means that it won't be sent to the swap area (disk). It will place
in a special area called `swap cache`.


## Reverse mapping

The kernel memory manager includes a `reverse mapping (rmap)` algorithm. When
a page is swapped, the page descriptor is updated to indicate that the page was
swapped. Because a page might be used by several processes, it is necessary to
be able to find all the proccesses given a page. This is where the `reverse mapping (rmap)`
algorithm comes into play.

Without reverse mapping the kernel has to examine every page it is considering
swapping out and then identify all processes currently using it, so it can update
the page-table entry for the page in that process.

## Out of Memory (OOM) Killer

Linux permits the system to overcommit memory, so that it can grant memory
requests that exceed the size of RAM plus swap area.

Linux kernel permits overcommission of memory only for user processes. For
kernel processes, the pages are not swappable and are always allocated at request
time.

The overcommision is set by `/proc/sys/vm/overcommit_memory`:

```shell
# 0: (default) Permit overcommission.
$ sudo sh -c 'echo 0 > /proc/sys/vm/overcommit_memory'
$ cat /proc/sys/vm/overcommit_memory
0

# 1: All memory requests are allowed to overcommit.
$ sudo sh -c 'echo 1 > /proc/sys/vm/overcommit_memory'
$ cat /proc/sys/vm/overcommit_memory
1

# 2: Turn off overcommission.
$ sudo sh -c 'echo 2 > /proc/sys/vm/overcommit_memory'
$ cat /proc/sys/vm/overcommit_memory
2
```

### Exercise: Invoking the OOM Killer

To invoke the OOM killer, we first have to turn off swapping and then request
more memory than what it is available.

```
# Turn off swapping
$ sudo /sbin/swapoff -a
```

> Note: When you are done, turn swapping on with:
> ```
> $ sudo /sbin/swapon -a
> ```

After the swap is off, stress the memory with

```
$ stress -m 10 -t 20
stress: info: [31784] dispatching hogs: 0 cpu, 0 io, 12 vm, 0 hdd
stress: info: [31784] successful run completed in 10s

```

In another terminal, see how the free memory is decreasing:

```
$ cat /proc/meminfo | grep MemFree
MemFree:         4636120 kB
$ cat /proc/meminfo | grep MemFree
MemFree:         4182900 kB 
```

Depending on how much memory your system has, the OOM Killer might not have been
called. In a system with 8GB, this will certainly kill several process:


```shell
$ stress -m 25 -t 50
stress: info: [31784] dispatching hogs: 0 cpu, 0 io, 25 vm, 0 hdd
```

This command will

- Fork off 25 memory-intensive processes, each spinning on malloc(), allocating 256 MB by default.
  The size can be changed as in --vm-bytes 128M.
- Run the stress test for 50 seconds.

To see the processes that were killed by the OOM Killer, see the kernel log
messages:

```
$ dmesg
[145485.698088] Out of memory: Kill process 4974 (stress) score 20 or sacrifice child
[145485.698092] Killed process 4974 (stress) total-vm:269628kB, anon-rss:153684kB, file-rss:0kB, shmem-rss:0kB
[145485.852419] ------------[ cut here ]------------
[145485.852936] WARNING: CPU: 1 PID: 2656 at /build/linux-hwe-EPHQQp/linux-hwe-4.15.0/drivers/gpu/drm/drm_gem.c:887 drm_gem_object_put+0x49/0x50 [drm]
[145485.852937] Modules linked in: uas usb_storage cdc_ether usbnet r8152 mii usblp prl_fs_freeze(POE) prl_fs(POE) prl_eth(POE) bnep x86_pkg_temp_thermal coretemp crct10dif_pclmul crc32_pclmul ghash_clmulni_intel pcbc snd_intel8x0 snd_ac97_codec ac97_bus snd_pcm aesni_intel aes_x86_64 snd_seq_midi crypto_simd gpio_ich glue_helper cryptd snd_seq_midi_event snd_rawmidi intel_rapl_perf btusb btrtl btbcm btintel snd_seq input_leds bluetooth snd_seq_device snd_timer serio_raw snd joydev ecdh_generic soundcore lpc_ich sbs pvpanic sbshc shpchp mac_hid binfmt_misc parport_pc ppdev lp parport autofs4 hid_generic usbhid hid prl_vid(POE) drm_kms_helper syscopyarea sysfillrect sysimgblt fb_sys_fops psmouse drm ahci prl_tg(POE) libahci pata_acpi virtio_net
[145485.852969] CPU: 1 PID: 2656 Comm: kworker/1:2 Tainted: P        W  OE    4.15.0-142-generic #146~16.04.1-Ubuntu
[145485.852969] Hardware name: Parallels Software International Inc. Parallels Virtual Platform/Parallels Virtual Platform, BIOS 16.5.1 (49187) 07/02/2021
[145485.852982] Workqueue: events drm_mode_rmfb_work_fn [drm]
[145485.852991] RIP: 0010:drm_gem_object_put+0x49/0x50 [drm]
[145485.852992] RSP: 0018:ffffc32646c5bd48 EFLAGS: 00010286
[145485.852993] RAX: 0000000000000024 RBX: ffffa01e82d97000 RCX: 0000000000000006
[145485.852994] RDX: 0000000000000000 RSI: ffffa01e86e56498 RDI: ffffa01e86e56490
[145485.852994] RBP: ffffc32646c5bd50 R08: 0000000000035cec R09: ffffffff8f5624c4
[145485.852995] R10: 0000000000000000 R11: 000000000001741c R12: ffffa01e7b8e5c00
[145485.852996] R13: ffffa01e7a1d0000 R14: 0000000000000006 R15: ffffa01e81bcd200
[145485.852997] FS:  0000000000000000(0000) GS:ffffa01e86e40000(0000) knlGS:0000000000000000
[145485.852998] CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
[145485.852999] CR2: 000055d287157aa0 CR3: 000000012500a004 CR4: 00000000003606e0
[145485.853005] DR0: 0000000000000000 DR1: 0000000000000000 DR2: 0000000000000000
[145485.853006] DR3: 0000000000000000 DR6: 00000000fffe0ff0 DR7: 0000000000000400
[145485.853007] Call Trace:
[145485.853015]  prl_fb_destroy+0xa9/0x130 [prl_vid]
[145485.853027]  drm_framebuffer_free+0x30/0x40 [drm]
[145485.853038]  drm_mode_object_put+0x4f/0x60 [drm]
[145485.853048]  drm_framebuffer_remove+0x283/0x3b0 [drm]
[145485.853056]  drm_mode_rmfb_work_fn+0x41/0x50 [drm]
[145485.853062]  process_one_work+0x14d/0x410
[145485.853063]  worker_thread+0x22b/0x460
[145485.853065]  kthread+0x105/0x140
[145485.853067]  ? process_one_work+0x410/0x410
[145485.853068]  ? kthread_bind+0x40/0x40
[145485.853071]  ret_from_fork+0x35/0x40
[145485.853072] Code: ff ff 74 1b f0 ff 0b 0f 88 c6 6b 02 00 74 04 5b 5d f3 c3 48 89 df e8 a7 f9 ff ff 5b 5d eb f2 48 c7 c7 68 30 18 c0 e8 ac e6 46 ce <0f> 0b eb d5 0f 1f 00 0f 1f 44 00 00 55 48 89 e5 41 57 41 56 41 
[145485.853084] ---[ end trace 08ff905db57248fa ]---
```
