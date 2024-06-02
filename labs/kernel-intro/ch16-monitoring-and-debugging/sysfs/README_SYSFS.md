
# sys filesystem

The patch in the current directory shows how to add a sys filesystem entry
in `/sys/kernel/mm`.

Once that the patch in this directory is apply to the top of the linux kernel,
and the kernel has been built and started, run:


```
 $ uname -a
Linux buildroot 6.6.14-00001-g5732bca7a26c-dirty #31 SMP PREEMPT Sat Jun  1 22:55:48 PDT 2024 aarch64 GNU/Linux

 $ cat /sys/kernel/mm/mm_info_group/mm_status
empty

 $ echo 1 > /sys/kernel/mm/mm_info_group/mm_status
 $ cat /sys/kernel/mm/mm_info_group/mm_status
half full

 $ echo 2 > /sys/kernel/mm/mm_info_group/mm_status
 $ cat /sys/kernel/mm/mm_info_group/mm_status
full

 $ echo 0 > /sys/kernel/mm/mm_info_group/mm_status
 $ cat /sys/kernel/mm/mm_info_group/mm_status
empty

 $ echo 20 > /sys/kernel/mm/mm_info_group/mm_status
sh: write error: Invalid argument
$ ls -la /sys/kernel/mm/mm_info_group/mm_status
-rw-r--r--    1 root     root          4096 Jun  2 06:02 /sys/kernel/mm/mm_info_group/mm_status
```

> Note: The patch was created by running this command in the kernel repository:
>
> ```
> $ git format-patch HEAD~1
> 0001-sys-Add-mm_info_group-dir-to-sys-kernel-mm-directory.patch
> ```
>
> To apply the patch, run:
>
> ```
> $ git apply 0001-sys-Add-mm_info_group-dir-to-sys-kernel-mm-directory.patch
> 0001-sys-Add-mm_info_group-dir-to-sys-kernel-mm-directory.patch:135: new blank line at EOF.
> +
> warning: 1 line adds whitespace errors.
> ```


