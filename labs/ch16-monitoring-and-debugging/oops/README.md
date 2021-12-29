

# oops message example


When the `oops_module.ko` is loaded, the kernel tries to deference a NULL pointer
and we get the output below:


```shell
$ dmesg
[ 7232.616219] oops_module: loading out-of-tree module taints kernel.
[ 7232.617171] oops_module: Loading the module oops_module at address 00000000453340fc
[ 7232.617238] Unable to handle kernel NULL pointer dereference at virtual address 0000000000000000
[ 7232.617259] Mem abort info:
[ 7232.617282]   ESR = 0x96000007
[ 7232.617304]   EC = 0x25: DABT (current EL), IL = 32 bits
[ 7232.617325]   SET = 0, FnV = 0
[ 7232.617349]   EA = 0, S1PTW = 0
[ 7232.617368] Data abort info:
[ 7232.617388]   ISV = 0, ISS = 0x00000007
[ 7232.617408]   CM = 0, WnR = 0
[ 7232.617431] user pgtable: 4k pages, 39-bit VAs, pgdp=0000000011fd7000
[ 7232.617455] [0000000000000000] pgd=0000000010399003, p4d=0000000010399003, pud=0000000010399003, pmd=0000000006242003, pte=0000000000000000
[ 7232.617562] Internal error: Oops: 96000007 [#1] PREEMPT SMP
[ 7232.617576] Modules linked in: oops_module(O+) cmac aes_arm64 rfcomm bnep hci_uart btbcm bluetooth ecdh_generic ecc fuse brcmfmac brcmutil sha256_generic cfg80211 raspberrypi_hwmon rfkill bcm2835_isp(C) bcm2835_codec(C) bcm2835_v4l2(C) snd_bcm2835(C) v4l2_mem2mem bcm2835_mmal_vchiq(C) videobuf2_vmalloc videobuf2_dma_contig videobuf2_memops videobuf2_v4l2 videobuf2_common snd_pcm sdhci_iproc videodev snd_timer mc snd vc_sm_cma(C) uio_pdrv_genirq uio i2c_dev ip_tables x_tables ipv6
[ 7232.617859] CPU: 1 PID: 1329 Comm: insmod Tainted: G         C O      5.10.75-v8+ #1
[ 7232.617871] Hardware name: Raspberry Pi 3 Model B Plus Rev 1.3 (DT)
[ 7232.617895] pstate: 80000005 (Nzcv daif -PAN -UAO -TCO BTYPE=--)
[ 7232.617920] pc : oops_module_init+0x3c/0x1000 [oops_module]
[ 7232.617939] lr : oops_module_init+0x30/0x1000 [oops_module]
[ 7232.617950] sp : ffffffc0106a3b00
[ 7232.617962] x29: ffffffc0106a3b00 x28: 0000000000000100
[ 7232.617985] x27: ffffff87823c1f30 x26: 0000000000000001
[ 7232.618008] x25: ffffff8790076500 x24: 0000000000000001
[ 7232.618030] x23: 0000000000000000 x22: ffffffda84918000
[ 7232.618053] x21: ffffffda68459050 x20: ffffffda68151000
[ 7232.618075] x19: ffffffda68459000 x18: ffffffda849186b8
[ 7232.618097] x17: 0000000000000000 x16: ffffffda84613970
[ 7232.618120] x15: ffffffc0906a37c7 x14: 616f4c203a656c75
[ 7232.618142] x13: 646f6d5f73706f6f x12: 20656c75646f6d20
[ 7232.618165] x11: 65687420676e6964 x10: 6120746120656c75
[ 7232.618188] x9 : 646f6d5f73706f6f x8 : 3430303030303030
[ 7232.618210] x7 : 0000000000000000 x6 : ffffff87b71a0a38
[ 7232.618232] x5 : ffffff87b71a0a38 x4 : ffffffad326ff000
[ 7232.618254] x3 : 0000000000000002 x2 : c7f701d70374b400
[ 7232.618276] x1 : 0000000000000000 x0 : ffffffda68458068
[ 7232.618299] Call trace:
[ 7232.618318]  oops_module_init+0x3c/0x1000 [oops_module]
[ 7232.618341]  do_one_initcall+0x54/0x2f0
[ 7232.618360]  do_init_module+0x60/0x238
[ 7232.618378]  load_module+0x2188/0x2730
[ 7232.618389]  __do_sys_finit_module+0xe8/0x100
[ 7232.618398]  __arm64_sys_finit_module+0x28/0x38
[ 7232.618410]  el0_svc_common+0x84/0x1c0
[ 7232.618420]  do_el0_svc_compat+0x30/0x60
[ 7232.618430]  el0_svc_compat+0x1c/0x28
[ 7232.618439]  el0_sync_compat_handler+0x9c/0xe8
[ 7232.618454]  el0_sync_compat+0x14c/0x180
[ 7232.618470] Code: 94000015 d2800001 f0001820 9101a000 (b9400021)
[ 7232.618480] ---[ end trace 4a6cf5fb4dc1b11b ]---
```

When the error happened, the program counter had:

```
 pc : oops_module_init+0x3c/0x1000 [oops_module]
```

If we see the instruction at address `0x3c` in `oops_module.txt`, which is
the objdump, we can see that is trying to deference the NULL pointer

```
30:	d2800001 	mov	x1, #0x0
    int *ptr = NULL;
3c:	b9400021 	ldr	w1, [x1]
 ```

To generate `oops_module.txt`, we run:

```shell
$ aarch64-linux-gnu-objdump -S -d oops_module.ko > oops_module.txt
```

