
# Allocating memory with kmalloc() and __get_free_pages()

When we try to allocate more than 4KB of continuous memory with kmalloc() and
__get_free_pages(), the request fails.

With `vmalloc()`, we can request up to 1GB~ till the request fails in a SD card
with 16GB of capacity.

```shell
pi@raspberrypi:~ $ df -h
Filesystem      Size  Used Avail Use% Mounted on
/dev/root        15G  3.2G   11G  23% /
devtmpfs        422M     0  422M   0% /dev
tmpfs           456M     0  456M   0% /dev/shm
tmpfs           456M   12M  444M   3% /run
tmpfs           5.0M  4.0K  5.0M   1% /run/lock
tmpfs           456M     0  456M   0% /sys/fs/cgroup
/dev/mmcblk0p1  253M   68M  185M  27% /boot
tmpfs            92M  4.0K   92M   1% /run/user/1000
tmpfs            92M     0   92M   0% /run/user/109
```

To see the logs after loading the module:

```shell
pi@raspberrypi:~ $ tail -f /var/log/messages
[ 7391.431768] Allocation memory with __get_free_pages
[ 7391.431786]  order= 0, pages=    1, size=    4096
[ 7391.431793] ... __get_free_pages OK
[ 7391.431801]  order= 1, pages=    2, size=    8192
[ 7391.431812] ... __get_free_pages OK
[ 7391.431824]  order= 2, pages=    4, size=   16384
[ 7391.431832] ... __get_free_pages OK
[ 7391.431841]  order= 3, pages=    8, size=   32768
[ 7391.431849] ... __get_free_pages OK
[ 7391.431864]  order= 4, pages=   16, size=   65536
[ 7391.431872] ... __get_free_pages OK
[ 7391.431882]  order= 5, pages=   32, size=  131072
[ 7391.431891] ... __get_free_pages OK
[ 7391.431900]  order= 6, pages=   64, size=  262144
[ 7391.431910] ... __get_free_pages OK
[ 7391.431919]  order= 7, pages=  128, size=  524288
[ 7391.431931] ... __get_free_pages OK
[ 7391.431941]  order= 8, pages=  256, size= 1048576
[ 7391.431957] ... __get_free_pages OK
[ 7391.431969]  order= 9, pages=  512, size= 2097152
[ 7391.431994] ... __get_free_pages OK
[ 7391.432008]  order=10, pages= 1024, size= 4194304
[ 7391.432048] ... __get_free_pages OK
[ 7391.432069]  order=11, pages= 2048, size= 8388608
[ 7391.432180] ------------[ cut here ]------------
[ 7391.432199] WARNING: CPU: 2 PID: 1218 at mm/page_alloc.c:4935 __alloc_pages_nodemask+0x2dc/0x338
[ 7391.432205] Modules linked in: lab_maxmem(O+) lab_cache(O) cmac aes_arm64 rfcomm bnep hci_uart btbcm bluetooth ecdh_generic ecc fuse brcmfmac brcmutil sha256_generic cfg80211 raspberrypi_hwmon rfkill bcm2835_codec(C) bcm2835_isp(C) bcm2835_v4l2(C) v4l2_mem2mem bcm2835_mmal_vchiq(C) videobuf2_vmalloc videobuf2_dma_contig videobuf2_memops videobuf2_v4l2 snd_bcm2835(C) videobuf2_common snd_pcm sdhci_iproc snd_timer videodev mc snd vc_sm_cma(C) uio_pdrv_genirq uio i2c_dev ip_tables x_tables ipv6 [last unloaded: lab_mempool]
[ 7391.432367] CPU: 2 PID: 1218 Comm: insmod Tainted: G         C O      5.10.75-v8+ #1
[ 7391.432373] Hardware name: Raspberry Pi 3 Model B Plus Rev 1.3 (DT)
[ 7391.432379] pstate: 20000005 (nzCv daif -PAN -UAO -TCO BTYPE=--)
[ 7391.432385] pc : __alloc_pages_nodemask+0x2dc/0x338
[ 7391.432390] lr : __get_free_pages+0x30/0x68
[ 7391.432394] sp : ffffffc010b0ba20
[ 7391.432398] x29: ffffffc010b0ba20 x28: 0000000000000100
[ 7391.432406] x27: ffffffbd447a4370 x26: 0000000000000001
[ 7391.432415] x25: ffffffbd45a07b00 x24: ffffffd28d867000
[ 7391.432424] x23: 0000000000000000 x22: 000000000000000b
[ 7391.432432] x21: ffffffd2aeb18000 x20: 000000000000000b
[ 7391.432440] x19: 0000000000000a20 x18: ffffffd2aeb186b8
[ 7391.432449] x17: 0000000000000000 x16: ffffffd2ae055a00
[ 7391.432457] x15: ffffffd2af120f78 x14: 0000000000000008
[ 7391.432465] x13: ffffffd2af120bbf x12: 0000000000000010
[ 7391.432474] x11: 0000000000000000 x10: 0000000005f5e0ff
[ 7391.432482] x9 : ffffffc010b0bad0 x8 : 2038303638383338
[ 7391.432490] x7 : 0000000000000000 x6 : ffffffbd771bfa38
[ 7391.432499] x5 : ffffffbd771bfa38 x4 : ffffffeac851e000
[ 7391.432507] x3 : 0000000000000000 x2 : 0000000000000000
[ 7391.432515] x1 : 0000000000000000 x0 : ffffffd2aeb186b8
[ 7391.432524] Call trace:
[ 7391.432529]  __alloc_pages_nodemask+0x2dc/0x338
[ 7391.432534]  __get_free_pages+0x30/0x68
[ 7391.432546]  my_init+0x80/0x1000 [lab_maxmem]
[ 7391.432561]  do_one_initcall+0x54/0x2f0
[ 7391.432573]  do_init_module+0x60/0x238
[ 7391.432584]  load_module+0x2188/0x2730
[ 7391.432592]  __do_sys_finit_module+0xe8/0x100
[ 7391.432600]  __arm64_sys_finit_module+0x28/0x38
[ 7391.432610]  el0_svc_common+0x84/0x1c0
[ 7391.432620]  do_el0_svc_compat+0x30/0x60
[ 7391.432628]  el0_svc_compat+0x1c/0x28
[ 7391.432636]  el0_sync_compat_handler+0x9c/0xe8
[ 7391.432644]  el0_sync_compat+0x14c/0x180
[ 7391.432650] ---[ end trace e7582159614cefa9 ]---
[ 7391.432673] ... __get_free_pages failed
[ 7391.432682] Allocation memory with __get_free_pages
[ 7391.432691]  order= 0, pages=    1, size=    4096
[ 7391.432700] ... kmalloc OK
[ 7391.432710]  order= 1, pages=    2, size=    8192
[ 7391.432717] ... kmalloc OK
[ 7391.432724]  order= 2, pages=    4, size=   16384
[ 7391.432740] ... kmalloc OK
[ 7391.432755]  order= 3, pages=    8, size=   32768
[ 7391.432764] ... kmalloc OK
[ 7391.432775]  order= 4, pages=   16, size=   65536
[ 7391.432784] ... kmalloc OK
[ 7391.432795]  order= 5, pages=   32, size=  131072
[ 7391.432806] ... kmalloc OK
[ 7391.432817]  order= 6, pages=   64, size=  262144
[ 7391.432829] ... kmalloc OK
[ 7391.432840]  order= 7, pages=  128, size=  524288
[ 7391.432858] ... kmalloc OK
[ 7391.432870]  order= 8, pages=  256, size= 1048576
[ 7391.432896] ... kmalloc OK
[ 7391.432910]  order= 9, pages=  512, size= 2097152
[ 7391.432946] ... kmalloc OK
[ 7391.432963]  order=10, pages= 1024, size= 4194304
[ 7391.433018] ... kmalloc OK
[ 7391.433041]  order=11, pages= 2048, size= 8388608
[ 7391.433116] ... kmalloc failed
[ 8343.021153]  order=?, pages=136192, size=557842432
[ 8343.242360] ... vmalloc OK
[ 8343.528565]  order=?, pages=137216, size=562036736
[ 8343.750986] ... vmalloc OK
[ 8344.036186]  order=?, pages=138240, size=566231040
[ 8344.261642] ... vmalloc OK
[ 8344.548927]  order=?, pages=139264, size=570425344
[ 8344.777079] ... vmalloc OK
[ 8345.070684]  order=?, pages=140288, size=574619648
[ 8345.298401] ... vmalloc OK
[ 8345.578159]  order=?, pages=141312, size=578813952
[ 8345.807466] ... vmalloc OK
[ 8346.101204]  order=?, pages=142336, size=583008256
[ 8346.332162] ... vmalloc OK
[ 8346.633156]  order=?, pages=143360, size=587202560
[ 8346.865332] ... vmalloc OK
[ 8347.163421]  order=?, pages=144384, size=591396864
[ 8347.397506] ... vmalloc OK
[ 8347.698016]  order=?, pages=145408, size=595591168
[ 8347.934688] ... vmalloc OK
[ 8348.237447]  order=?, pages=146432, size=599785472
[ 8348.475126] ... vmalloc OK
[ 8348.779622]  order=?, pages=147456, size=603979776
[ 8349.018629] ... vmalloc OK
[ 8349.325454]  order=?, pages=148480, size=608174080
[ 8349.565943] ... vmalloc OK
[ 8349.874706]  order=?, pages=149504, size=612368384
[ 8350.117182] ... vmalloc OK
[ 8350.427705]  order=?, pages=150528, size=616562688
[ 8350.672413] ... vmalloc OK
[ 8350.985996]  order=?, pages=151552, size=620756992
[ 8351.231934] ... vmalloc OK
[ 8351.546993]  order=?, pages=152576, size=624951296
[ 8351.794251] ... vmalloc OK
[ 8352.111213]  order=?, pages=153600, size=629145600
[ 8352.360489] ... vmalloc OK
[ 8352.680188]  order=?, pages=154624, size=633339904
[ 8352.931001] ... vmalloc OK
[ 8353.252438]  order=?, pages=155648, size=637534208
[ 8353.504916] ... vmalloc OK
[ 8353.828461]  order=?, pages=156672, size=641728512
[ 8354.082519] ... vmalloc OK
[ 8354.408621]  order=?, pages=157696, size=645922816
[ 8354.664388] ... vmalloc OK
[ 8354.992317]  order=?, pages=158720, size=650117120
[ 8355.249402] ... vmalloc OK
[ 8355.579666]  order=?, pages=159744, size=654311424
[ 8355.838820] ... vmalloc OK
[ 8356.170882]  order=?, pages=160768, size=658505728
[ 8356.430996] ... vmalloc OK
[ 8356.765122]  order=?, pages=161792, size=662700032
[ 8357.026462] ... vmalloc OK
[ 8357.362656]  order=?, pages=162816, size=666894336
[ 8357.626778] ... vmalloc OK
[ 8357.965190]  order=?, pages=163840, size=671088640
[ 8358.231043] ... vmalloc OK
[ 8358.571707]  order=?, pages=164864, size=675282944
[ 8358.856799] ... vmalloc OK
[ 8359.199811]  order=?, pages=165888, size=679477248
[ 8359.469138] ... vmalloc OK
[ 8359.813958]  order=?, pages=166912, size=683671552
[ 8360.084869] ... vmalloc OK
[ 8360.433697]  order=?, pages=167936, size=687865856
[ 8360.706392] ... vmalloc OK
[ 8361.055857]  order=?, pages=168960, size=692060160
[ 8361.392445] ... vmalloc OK
[ 8361.743929]  order=?, pages=169984, size=696254464
[ 8362.019756] ... vmalloc OK
[ 8362.372844]  order=?, pages=171008, size=700448768
[ 8362.650741] ... vmalloc OK
[ 8363.006346]  order=?, pages=172032, size=704643072
[ 8363.285547] ... vmalloc OK
....
[ 8428.513485]  order=?, pages=201728, size=826277888
[ 8430.580234] insmod invoked oom-killer: gfp_mask=0x2cc2(GFP_KERNEL|__GFP_HIGHMEM|__GFP_NOWARN), order=0, oom_score_adj=0
[ 8430.580261] CPU: 3 PID: 1260 Comm: insmod Tainted: G        WC O      5.10.75-v8+ #1
[ 8430.580266] Hardware name: Raspberry Pi 3 Model B Plus Rev 1.3 (DT)
[ 8430.580271] Call trace:
[ 8430.580287]  dump_backtrace+0x0/0x1b8
[ 8430.580294]  show_stack+0x2c/0x38
[ 8430.580304]  dump_stack+0xec/0x150
[ 8430.580313]  dump_header+0x48/0x218
[ 8430.580319]  oom_kill_process+0x14c/0x228
[ 8430.580326]  out_of_memory+0xec/0x358
[ 8430.580334]  __alloc_pages_slowpath.constprop.131+0x948/0xc30
[ 8430.580340]  __alloc_pages_nodemask+0x2bc/0x338
[ 8430.580347]  __vmalloc_node_range+0x180/0x2c8
[ 8430.580354]  __vmalloc_node+0x80/0x98
[ 8430.580360]  vmalloc+0x70/0x80
[ 8430.580374]  my_init+0x194/0x1000 [lab_maxmem]
[ 8430.580381]  do_one_initcall+0x54/0x2f0
[ 8430.580388]  do_init_module+0x60/0x238
[ 8430.580396]  load_module+0x2188/0x2730
[ 8430.580402]  __do_sys_finit_module+0xe8/0x100
[ 8430.580409]  __arm64_sys_finit_module+0x28/0x38
[ 8430.580416]  el0_svc_common+0x84/0x1c0
[ 8430.580423]  do_el0_svc_compat+0x30/0x60
[ 8430.580430]  el0_svc_compat+0x1c/0x28
[ 8430.580437]  el0_sync_compat_handler+0x9c/0xe8
[ 8430.580442]  el0_sync_compat+0x14c/0x180
[ 8430.580448] Mem-Info:
[ 8430.580464] active_anon:11 inactive_anon:21 isolated_anon:0
[ 8430.580464]  active_file:25 inactive_file:153 isolated_file:0
[ 8430.580464]  unevictable:4 dirty:0 writeback:0
[ 8430.580464]  slab_reclaimable:3874 slab_unreclaimable:5512
[ 8430.580464]  mapped:31 shmem:0 pagetables:594 bounce:0
[ 8430.580464]  free:16669 free_pcp:186 free_cma:14759
[ 8430.580477] Node 0 active_anon:44kB inactive_anon:84kB active_file:100kB inactive_file:612kB unevictable:16kB isolated(anon):0kB isolated(file):0kB mapped:124kB dirty:0kB writeback:0kB shmem:0kB writeback_tmp:0kB kernel_stack:2656kB all_unreclaimable? no
```