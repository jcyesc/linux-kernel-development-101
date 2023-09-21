
# How to debug a module

This examples shows how to:

- understand the kernel panic messages.
- find the line of code that cause the issue.


## Debugging

When the module is installed, a segmentation fault occurrs:

```shell
pi@raspberrypi:~ $ sudo insmod module_with_errors.ko

Message from syslogd@raspberrypi at Jul  4 18:58:26 ...
 kernel:[   67.871567] Internal error: Oops: 96000007 [#1] PREEMPT SMP

Message from syslogd@raspberrypi at Jul  4 18:58:26 ...
 kernel:[   67.872055] Code: d297dde2 b0001220 9100a000 52800241 (b9400042)
Segmentation fault
```

This part of the message `Oops: 96000007 [#1] PREEMPT SMP` indicates that
this is the first `Oops. It is important to notice that one Oops can generate
more oopses.

The `96000007` it is the content of the Exception Syndrome Register in ARM:

- https://developer.arm.com/documentation/ddi0601/2022-03/AArch64-Registers/ESR-EL1--Exception-Syndrome-Register--EL1-


In the kernel logs, we can see:


```shell
pi@raspberrypi:~ $ dmesg
...
...
[   67.870886] module_with_errors: loading out-of-tree module taints kernel.
[   67.871409] Unable to handle kernel paging request at virtual address 000000000000beef
[   67.871428] Mem abort info:
[   67.871438]   ESR = 0x96000007
[   67.871449]   EC = 0x25: DABT (current EL), IL = 32 bits
[   67.871463]   SET = 0, FnV = 0
[   67.871473]   EA = 0, S1PTW = 0
[   67.871482] Data abort info:
[   67.871491]   ISV = 0, ISS = 0x00000007
[   67.871501]   CM = 0, WnR = 0
[   67.871512] user pgtable: 4k pages, 39-bit VAs, pgdp=0000000012802000
[   67.871522] [000000000000beef] pgd=00000000144ae003, p4d=00000000144ae003, pud=00000000144ae003, pmd=0000000011b63003, pte=0000000000000000
[   67.871567] Internal error: Oops: 96000007 [#1] PREEMPT SMP
[   67.871576] Modules linked in: module_with_errors(O+) cmac aes_arm64 rfcomm bnep hci_uart btbcm bluetooth ecdh_generic ecc fuse brcmfmac brcmutil sha256_generic cfg80211 rfkill raspberrypi_hwmon bcm2835_codec(C) bcm2835_isp(C) bcm2835_v4l2(C) v4l2_mem2mem bcm2835_mmal_vchiq(C) videobuf2_dma_contig sdhci_iproc videobuf2_vmalloc videobuf2_memops snd_bcm2835(C) videobuf2_v4l2 videobuf2_common snd_pcm snd_timer snd videodev vc_sm_cma(C) mc uio_pdrv_genirq uio i2c_dev ip_tables x_tables ipv6
[   67.871722] CPU: 0 PID: 868 Comm: insmod Tainted: G         C O      5.10.75-v8 #4
[   67.871728] Hardware name: Raspberry Pi 3 Model B Plus Rev 1.3 (DT)
[   67.871736] pstate: 40000005 (nZcv daif -PAN -UAO -TCO BTYPE=--)
[   67.871748] pc : module_with_errors_init+0x20/0x1000 [module_with_errors]
[   67.871761] lr : do_one_initcall+0x54/0x2f0
[   67.871767] sp : ffffffc010c63b00
[   67.871772] x29: ffffffc010c63b00 x28: 0000000000000100
[   67.871784] x27: ffffff8ed2a15730 x26: 0000000000000001
[   67.871796] x25: ffffff8ed2a9ae80 x24: 0000000000000001
[   67.871807] x23: 0000000000000000 x22: ffffffd55d31b000
[   67.871818] x21: ffffffd4e85ae050 x20: ffffffd4e8368000
[   67.871830] x19: ffffffd4e85ae000 x18: 0000000000000000
[   67.871841] x17: 0000000000000000 x16: 0000000000000000
[   67.871853] x15: ffffffffffffffff x14: 0000000000000000
[   67.871864] x13: ffffff8ec35c3e90 x12: 0000000000000010
[   67.871875] x11: 0101010101010101 x10: 7f7f7f7f7f7f7f7f
[   67.871887] x9 : ffffff8ed2a9a000 x8 : ffffff8ed2a9af80
[   67.871898] x7 : 000000000003a2c0 x6 : ffffffd55d73b000
[   67.871910] x5 : ffffffd55c84e9c4 x4 : ffffffc010c63b90
[   67.871922] x3 : 0000000000000001 x2 : 000000000000beef
[   67.871933] x1 : 0000000000000012 x0 : ffffffd4e85ad028
[   67.871945] Call trace:
[   67.871955]  module_with_errors_init+0x20/0x1000 [module_with_errors]
[   67.871963]  do_one_initcall+0x54/0x2f0
[   67.871974]  do_init_module+0x60/0x238
[   67.871986]  load_module+0x2188/0x2730
[   67.871994]  __do_sys_finit_module+0xe8/0x100
[   67.872002]  __arm64_sys_finit_module+0x28/0x38
[   67.872012]  el0_svc_common+0x84/0x1c0
[   67.872020]  do_el0_svc_compat+0x30/0x60
[   67.872029]  el0_svc_compat+0x1c/0x28
[   67.872037]  el0_sync_compat_handler+0x9c/0xe8
[   67.872044]  el0_sync_compat+0x14c/0x180
[   67.872055] Code: d297dde2 b0001220 9100a000 52800241 (b9400042)
[   67.872064] ---[ end trace d7e8d759c91cd63d ]---
```

The messsage `pc : module_with_errors_init+0x20/0x1000 [module_with_errors]`
indicates that the instruction that cause the issue is in the offset `0x20` from
the beginning of the function `module_with_errors_init`.

To find the instruction, we use `aarch64-linux-gnu-objdump`.

```shell
$ aarch64-linux-gnu-objdump -dS --adjust-vma=0x0 module_with_errors.ko

module_with_errors.ko:     file format elf64-littleaarch64


Disassembly of section .init.text:

0000000000000000 <init_module>:
   0:	a9bf7bfd 	stp	x29, x30, [sp,#-16]!
   4:	910003fd 	mov	x29, sp
   8:	aa1e03e0 	mov	x0, x30
   c:	94000000 	bl	0 <_mcount>
  10:	d297dde2 	mov	x2, #0xbeef                	// #48879
  14:	90000000 	adrp	x0, 0 <init_module>
  18:	91000000 	add	x0, x0, #0x0
  1c:	52800241 	mov	w1, #0x12                  	// #18
  20:	b9400042 	ldr	w2, [x2]
  24:	94000000 	bl	0 <printk>
  28:	52800000 	mov	w0, #0x0                   	// #0
  2c:	a8c17bfd 	ldp	x29, x30, [sp],#16
  30:	d65f03c0 	ret

Disassembly of section .exit.text:

0000000000000000 <cleanup_module>:
   0:	a9bf7bfd 	stp	x29, x30, [sp,#-16]!
   4:	90000000 	adrp	x0, 0 <cleanup_module>
   8:	91000000 	add	x0, x0, #0x0
   c:	910003fd 	mov	x29, sp
  10:	94000000 	bl	0 <printk>
  14:	a8c17bfd 	ldp	x29, x30, [sp],#16
  18:	d65f03c0 	ret
```

After looking at instructions 0x10 and 0x20, we can see that error happens
when the address 0xbeef is being deferenced.


```shell
  10:	d297dde2 	mov	x2, #0xbeef                	// #48879
  14:	90000000 	adrp	x0, 0 <init_module>
  18:	91000000 	add	x0, x0, #0x0
  1c:	52800241 	mov	w1, #0x12                  	// #18
  20:	b9400042 	ldr	w2, [x2]
```

The `#18` indicates the line in the file. We can also find the line by using
`aarch64-linux-gnu-addr2line`.

```shell
$ aarch64-linux-gnu-addr2line -e module_with_errors.o 0x20
/home/parallels/Development/linux-kernel-development-101/labs/kernel-intro/ch16-monitoring-and-debugging/debug_module/module_with_errors.c:18
```

> Note: For `aarch64-linux-gnu-addr2line` to work, it is necessary to
> set these flags in the `Makefile`:
> EXTRA_CFLAGS = -Wall -g

