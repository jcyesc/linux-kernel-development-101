# vm_area_struct and anon mappings

The program `vm_anon_name.cpp` shows:

 - how to use mmap
 - set a name for the vm_area_struct
 - print the vm areas using /proc/self/maps

In order to set a name for a vm_area_struct:

 - the Linux Kernel has to be at least `5.17`.
 - `CONFIG_ANON_VMA_NAME=y` has to be set.

To compile and run the program, follow steps below:

```shell
host $ aarch64-linux-gnu-g++ -o vm_anon_name vm_anon_name.cpp
host $ scp -P 8022 vm_anon_name root@127.0.0.1:~
host $ ssh root@127.0.0.1 -p 8022


# ./vm_anon_name
Memory map a region and print vm_area_struct
PID: 245

Press enter to mmap - cat /proc/245/maps > maps_1_start.txt

mmap addr: 0xffffadaf5000 size: 4096

Press enter to set vm_area_struct's name - cat /proc/245/maps > maps_2_mmap.txt

Set [0xffffadaf5000, 0xffffadaf6000] = /secret/mem/area

Press enter to unmmap the region - cat /proc/245/maps > maps_3_set_vm_name.txt

Press enter to exit - cat /proc/245/maps > maps_4_ummap.txt
```

After running the program and saving the `/proc/<pid>/maps` output at
different stage of the execution, we can see that when the program starts,
the anon region goes from `ffffadaf6000-ffffadafc000`. However, after
mmap a 4096 region, the anon region now goes from `ffffadaf5000-ffffadafc000`,
which it is a 4096 increase.


```shell
 $ diff maps_1_start.txt maps_2_mmap.txt
25c25
< ffffadaf6000-ffffadafc000 rw-p 00000000 00:00 0
---
> ffffadaf5000-ffffadafc000 rw-p 00000000 00:00 0
```

After setting the name `/secret/mem/area` to the region `ffffadaf5000-ffffadaf6000`,
we can see that this region `ffffadaf5000-ffffadafc000` was splitted in 2:

```shell
 $ diff maps_2_mmap.txt maps_3_set_vm_name.txt
25c25,26
< ffffadaf5000-ffffadafc000 rw-p 00000000 00:00 0
---
> ffffadaf5000-ffffadaf6000 rw-p 00000000 00:00 0                          [anon:/secret/mem/area]
> ffffadaf6000-ffffadafc000 rw-p 00000000 00:00 0
```

When we unmmap the region, we can see anon mapping [anon:/secret/mem/area] is
not present anymore.

```shell
 $ diff maps_3_set_vm_name.txt maps_4_ummap.txt
25d24
< ffffadaf5000-ffffadaf6000 rw-p 00000000 00:00 0                          [anon:/secret/mem/area]
```

If we compare the initial memory regions vs the final memory regions, we can
see that they are the same.

```shell
 $ diff maps_1_start.txt maps_4_ummap.txt
```

