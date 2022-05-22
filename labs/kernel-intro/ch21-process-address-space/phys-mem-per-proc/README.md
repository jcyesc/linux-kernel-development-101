
# Print the physical and virtual memory addresses for a process


Given a pid, the physical and virtual memory addresses will be printed. In
order to make this exercise interesting, the `fork_userspace.c` file creates
a child process and prints the pid of the parent and child. Then, we use
the `phys_mem_per_proc.ko` module to print the memory and then we compare
the physical memory of the parent and child.

> Note: The output is from a Raspberry Pi +3b.

The content of this directory is:

```
Makefile                     Rules to build the module.
phys_mem_per_proc.c          Module that prints physical and vm.
fork_userspace.c             User space app that fork() a process.

child-memory.txt             Dump of the child memory at running time.
parent-memory.txt            Dump of the parent memory at running time.
fork_userspace-objdump.txt   Memory dump of the ELF file.

README.md                    Contains instructions to build and deploy module.
```

# Compile and dump the ELF file

```shell
$ arm-linux-gnueabi-gcc -o fork_userspace fork_userspace.c

$ file fork_userspace
fork_userspace: ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux.so.3, for GNU/Linux 3.2.0, BuildID[sha1]=864c24897fdd29bde9cd63e6e836fd0770efd2a7, not stripped

$ ls -lh fork_userspace
-rwxrwxr-x 1 parallels parallels 8.5K May 21 08:31 fork_userspace

# Upload the program to RPi
$ scp fork_userspace pi@192.168.2.2:~

$ arm-linux-gnueabi-objdump -D fork_userspace

fork_userspace:     file format elf32-littlearm

Disassembly of section .init:

00010428 <_init>:
   10428:	e92d4008 	push	{r3, lr}
   1042c:	eb000035 	bl	10508 <call_weak_fn>
   10430:	e8bd8008 	pop	{r3, pc}

Disassembly of section .text:

000104cc <_start>:
   104cc:	e3a0b000 	mov	fp, #0
   104d0:	e3a0e000 	mov	lr, #0
   104d4:	e49d1004 	pop	{r1}		; (ldr r1, [sp], #4)
   104d8:	e1a0200d 	mov	r2, sp
   104dc:	e52d2004 	push	{r2}		; (str r2, [sp, #-4]!)
   104e0:	e52d0004 	push	{r0}		; (str r0, [sp, #-4]!)
   104e4:	e59fc010 	ldr	ip, [pc, #16]	; 104fc <_start+0x30>
   104e8:	e52dc004 	push	{ip}		; (str ip, [sp, #-4]!)
   104ec:	e59f000c 	ldr	r0, [pc, #12]	; 10500 <_start+0x34>
   104f0:	e59f300c 	ldr	r3, [pc, #12]	; 10504 <_start+0x38>
   104f4:	ebffffe2 	bl	10484 <__libc_start_main@plt>
   104f8:	ebfffff0 	bl	104c0 <abort@plt>
   104fc:	00010774 	andeq	r0, r1, r4, ror r7
   10500:	000105f4 	strdeq	r0, [r1], -r4
   10504:	00010714 	andeq	r0, r1, r4, lsl r7

000105f4 <main>:
   105f4:	e92d4800 	push	{fp, lr}
   105f8:	e28db004 	add	fp, sp, #4
   105fc:	e24dd018 	sub	sp, sp, #24
   10600:	e59f30f4 	ldr	r3, [pc, #244]	; 106fc <main+0x108>
   10604:	e5933000 	ldr	r3, [r3]
   10608:	e50b3008 	str	r3, [fp, #-8]
   1060c:	ebffffa8 	bl	104b4 <fork@plt>
   10610:	e1a03000 	mov	r3, r0
   10614:	e50b300c 	str	r3, [fp, #-12]
   10618:	e51b300c 	ldr	r3, [fp, #-12]
   1061c:	e3530000 	cmp	r3, #0
   10620:	1a000014 	bne	10678 <main+0x84>
   10624:	ebffff9c 	bl	1049c <getpid@plt>
   10628:	e1a03000 	mov	r3, r0
   1062c:	e1a01003 	mov	r1, r3
   10630:	e59f00c8 	ldr	r0, [pc, #200]	; 10700 <main+0x10c>
   10634:	ebffff83 	bl	10448 <printf@plt>
   10638:	ebffff85 	bl	10454 <getchar@plt>
   1063c:	e50b0010 	str	r0, [fp, #-16]
   10640:	ea000007 	b	10664 <main+0x70>
   10644:	e51b3010 	ldr	r3, [fp, #-16]
   10648:	e353004d 	cmp	r3, #77	; 0x4d
   1064c:	1a000002 	bne	1065c <main+0x68>
   10650:	e59f30ac 	ldr	r3, [pc, #172]	; 10704 <main+0x110>
   10654:	e3a02054 	mov	r2, #84	; 0x54
   10658:	e5c32000 	strb	r2, [r3]
   1065c:	ebffff7c 	bl	10454 <getchar@plt>
   10660:	e50b0010 	str	r0, [fp, #-16]
   10664:	e51b3010 	ldr	r3, [fp, #-16]
   10668:	e3530045 	cmp	r3, #69	; 0x45
   1066c:	1afffff4 	bne	10644 <main+0x50>
   10670:	e3a00000 	mov	r0, #0
   10674:	ebffff8b 	bl	104a8 <exit@plt>
   10678:	e51b300c 	ldr	r3, [fp, #-12]
   1067c:	e3530000 	cmp	r3, #0
   10680:	da000011 	ble	106cc <main+0xd8>

Disassembly of section .rodata:

00010780 <_IO_stdin_used>:
   10780:	00020001 	andeq	r0, r2, r1
   10784:	6c696863 	stclvs	8, cr6, [r9], #-396	; 0xfffffe74
   10788:	43203a64 			; <UNDEFINED> instruction: 0x43203a64
   1078c:	646c6968 	strbtvs	r6, [ip], #-2408	; 0xfffff698
   10790:	4950202c 	ldmdbmi	r0, {r2, r3, r5, sp}^
   10794:	64252044 	strtvs	r2, [r5], #-68	; 0xffffffbc
   10798:	0000000a 	andeq	r0, r0, sl
   1079c:	65726170 	ldrbvs	r6, [r2, #-368]!	; 0xfffffe90
   107a0:	203a746e 	eorscs	r7, sl, lr, ror #8
   107a4:	65726150 	ldrbvs	r6, [r2, #-336]!	; 0xfffffeb0
   107a8:	5020746e 	eorpl	r7, r0, lr, ror #8
   107ac:	25204449 	strcs	r4, [r0, #-1097]!	; 0xfffffbb7
   107b0:	43202c64 			; <UNDEFINED> instruction: 0x43202

Disassembly of section .data:

0002103c <__data_start>:
   2103c:	00000000 	andeq	r0, r0, r0

00021040 <__dso_handle>:
   21040:	00000000 	andeq	r0, r0, r0

Disassembly of section .bss:

00022000 <__stack_chk_guard@@GLIBC_2.4>:
   22000:	00000000 	andeq	r0, r0, r0

00022004 <completed.9905>:
	...

00023000 <buffer>:
	...
```

## Compile and deploy the driver

```shell
# Clean the module
$ perl ~/linux/scripts/checkpatch.pl -f phys_mem_per_proc.c
total: 0 errors, 0 warnings, 217 lines checked

phys_mem_per_proc.c has no obvious style problems and is ready for submission.
$ make
$ make deploy

# Install driver
$ ssh pi@192.168.2.2
pi@raspberrypi:~ $ sudo insmod phys_mem_per_proc.ko

# Create the device node.
pi@raspberrypi:~ $ sudo mknod -m 666 /dev/physmem c 500 0

# Check that the device is registered.
pi@raspberrypi:~ $ cat /proc/devices | grep physmem
500 physmem
pi@raspberrypi:~ $ lsmod
Module                  Size  Used by
phys_mem_per_proc      16384  0
cmac                   16384  1
```

# Execute the program and print the memory


Execute the program and don't type anything:

```shell
pi@raspberrypi:~ $  ./fork_userspace
parent: Parent PID 863, Child PID 864
child: Child, PID 864
```

In another console print the memory for the parent and child and verify that
`copy-on-write` hasn't been done:

```shell
pi@raspberrypi:~ $ echo 863 > /dev/physmem
pi@raspberrypi:~ $ echo 864 > /dev/physmem
pi@raspberrypi:~ $ dmesg
```

## Parent Memory

```
[   78.605553] Page information for PID [863]
[   78.605577]  # vmas         vma(ptr)        start          end       length(Hex=Decimal=KB)     RWESH
[   78.605608]      1:         de49375d        10000        11000       1000 =    4096 =      4KB  R-E--
[   78.605629]      2:         c5e4c977        20000        21000       1000 =    4096 =      4KB  R----
[   78.605650]      3:         15e9fc8c        21000        22000       1000 =    4096 =      4KB  RW---
[   78.605893]      4:         6890388d        22000        26000       4000 =   16384 =     16KB  RW---
[   78.605915]      5:         882392d1       107000       128000      21000 =  135168 =    135KB  RW---
[   78.605936]      6:         7bf1cbca     F7A00000     F7B38000     138000 = 1277952 =   1277KB  R-E--
[   78.605957]      7:         adacb148     F7B38000     F7B48000      10000 =   65536 =     65KB  -----
[   78.605977]      8:         6b22e567     F7B48000     F7B4A000       2000 =    8192 =      8KB  R----
[   78.605998]      9:         c79c4fb1     F7B4A000     F7B4B000       1000 =    4096 =      4KB  RW---
[   78.606018]     10:         11fc2915     F7B4B000     F7B4E000       3000 =   12288 =     12KB  RW---
[   78.606051]     11:         e9d40903     F7B62000     F7B66000       4000 =   16384 =     16KB  R-E--
[   78.606072]     12:         bdf5bc06     F7B66000     F7B75000       F000 =   61440 =     61KB  -----
[   78.606193]     13:         f83ba0c5     F7B75000     F7B76000       1000 =    4096 =      4KB  R----
[   78.606222]     14:         ca78f8a9     F7B76000     F7B77000       1000 =    4096 =      4KB  RW---
[   78.606243]     15:         38513afe     F7B77000     F7B97000      20000 =  131072 =    131KB  R-E--
[   78.606264]     16:         891ca4aa     F7BA3000     F7BA5000       2000 =    8192 =      8KB  RW---
[   78.606285]     17:         f7f97a65     F7BA6000     F7BA7000       1000 =    4096 =      4KB  R-E--
[   78.606306]     18:         5ce50e05     F7BA7000     F7BA8000       1000 =    4096 =      4KB  R----
[   78.606327]     19:         3371d847     F7BA8000     F7BA9000       1000 =    4096 =      4KB  RW---
[   78.606365]     20:         1f0e336c     FFB8D000     FFBAE000      21000 =  135168 =    135KB  RW---
[   78.606385]     21:         a714764d     FFFF0000     FFFF1000       1000 =    4096 =      4KB  R-E--
[   78.606399]  
[   78.606414]          Physical Address       Virtual Address
[   78.606435]            11323003 (PGD)                 10000
[   78.606454]      20000013A2DFC3 (PTE)                 10000
[   78.606467]  
[   78.606481]          Physical Address       Virtual Address
[   78.606497]            11323003 (PGD)                 20000
[   78.606513]      E0000004A82FC3 (PTE)                 20000
[   78.606538]  
[   78.606552]          Physical Address       Virtual Address
[   78.606568]            11323003 (PGD)                 21000
[   78.606584]      E800000500BF43 (PTE)                 21000
[   78.606597]  
[   78.606611]          Physical Address       Virtual Address
[   78.606626]            11323003 (PGD)                 22000
[   78.606641]      E000000498FFC3 (PTE)                 22000
[   78.606655]                   0 (PTE)                 23000
[   78.606669]                   0 (PTE)                 24000
[   78.606683]                   0 (PTE)                 25000
```

## Child memory

```
[   82.564689] Page information for PID [864]
[   82.564704]  # vmas         vma(ptr)        start          end       length(Hex=Decimal=KB)     RWESH
[   82.564718]      1:         b7131ecc        10000        11000       1000 =    4096 =      4KB  R-E--
[   82.564728]      2:         bae1b8f1        20000        21000       1000 =    4096 =      4KB  R----
[   82.564737]      3:         9b93775c        21000        22000       1000 =    4096 =      4KB  RW---
[   82.564746]      4:         65dd6410        22000        26000       4000 =   16384 =     16KB  RW---
[   82.564756]      5:         de319011       107000       128000      21000 =  135168 =    135KB  RW---
[   82.564765]      6:         7d447791     F7A00000     F7B38000     138000 = 1277952 =   1277KB  R-E--
[   82.564774]      7:         e3c7653e     F7B38000     F7B48000      10000 =   65536 =     65KB  -----
[   82.564783]      8:         37f08a01     F7B48000     F7B4A000       2000 =    8192 =      8KB  R----
[   82.564792]      9:         24bbae2b     F7B4A000     F7B4B000       1000 =    4096 =      4KB  RW---
[   82.564801]     10:         3afcf48e     F7B4B000     F7B4E000       3000 =   12288 =     12KB  RW---
[   82.564811]     11:         34ac0b54     F7B62000     F7B66000       4000 =   16384 =     16KB  R-E--
[   82.564820]     12:         751cccf4     F7B66000     F7B75000       F000 =   61440 =     61KB  -----
[   82.564829]     13:          58fa35c     F7B75000     F7B76000       1000 =    4096 =      4KB  R----
[   82.564838]     14:          2a32bde     F7B76000     F7B77000       1000 =    4096 =      4KB  RW---
[   82.564847]     15:         eac046c7     F7B77000     F7B97000      20000 =  131072 =    131KB  R-E--
[   82.564856]     16:         7df6194f     F7BA3000     F7BA5000       2000 =    8192 =      8KB  RW---
[   82.564865]     17:         7a792cce     F7BA6000     F7BA7000       1000 =    4096 =      4KB  R-E--
[   82.564874]     18:         f840c873     F7BA7000     F7BA8000       1000 =    4096 =      4KB  R----
[   82.564883]     19:         b563bf4f     F7BA8000     F7BA9000       1000 =    4096 =      4KB  RW---
[   82.564892]     20:         4948ac3d     FFB8D000     FFBAE000      21000 =  135168 =    135KB  RW---
[   82.564901]     21:         16112989     FFFF0000     FFFF1000       1000 =    4096 =      4KB  R-E--
[   82.564907]  
[   82.564913]          Physical Address       Virtual Address
[   82.564922]            125C9003 (PGD)                 10000
[   82.564931]      20000013A2DFC3 (PTE)                 10000
[   82.564937]  
[   82.564943]          Physical Address       Virtual Address
[   82.564950]            125C9003 (PGD)                 20000
[   82.564957]      E0000004A82FC3 (PTE)                 20000
[   82.564963]  
[   82.564968]          Physical Address       Virtual Address
[   82.564975]            125C9003 (PGD)                 21000
[   82.564982]      E8000013DDAF43 (PTE)                 21000
[   82.564988]  
[   82.564994]          Physical Address       Virtual Address
[   82.565000]            125C9003 (PGD)                 22000
[   82.565007]      E000000498FFC3 (PTE)                 22000
[   82.565013]                   0 (PTE)                 23000
[   82.565025]                   0 (PTE)                 24000
[   82.565031]                   0 (PTE)                 25000
```

## Important things to `note`

- The first 2 pages for the parent and child point to the same physical page tables:

```
20000013A2DFC3 (PTE)                 10000
E0000004A82FC3 (PTE)                 20000
```

- The 3rd page in the parent and child point to different physical page tables,
however, the 4th page in the parent and child point to the same physical page.

```
# Parent memory
[   78.606552]          Physical Address       Virtual Address
[   78.606568]            11323003 (PGD)                 21000
[   78.606584]      E800000500BF43 (PTE)                 21000
[   78.606597]  
[   78.606611]          Physical Address       Virtual Address
[   78.606626]            11323003 (PGD)                 22000
[   78.606641]      E000000498FFC3 (PTE)                 22000
[   78.606655]                   0 (PTE)                 23000
[   78.606669]                   0 (PTE)                 24000
[   78.606683]                   0 (PTE)                 25000

# Child Memory
[   82.564968]          Physical Address       Virtual Address
[   82.564975]            125C9003 (PGD)                 21000
[   82.564982]      E8000013DDAF43 (PTE)                 21000
[   82.564988]  
[   82.564994]          Physical Address       Virtual Address
[   82.565000]            125C9003 (PGD)                 22000
[   82.565007]      E000000498FFC3 (PTE)                 22000
[   82.565013]                   0 (PTE)                 23000
[   82.565025]                   0 (PTE)                 24000
[   82.565031]                   0 (PTE)                 25000
```

What is in the vitual pages `0x21000`, `0x22000`, `0x23000`, `0x24000` and
`0x25000`? As per the objdump of the userspace program in the virtual page `0x21000`
we find the `stack`. The `buffer` was placed in`00023000 <buffer>:` and the
`.bss` section starts at `0x00022000`.


As soon as we type `M` (from Modify) in the user space program, we can see that
physical page has been allocated in the child process:

```shell
pi@raspberrypi:~ $ ./fork_userspace 
parent: Parent PID 863, Child PID 864
child: Child, PID 864
M
```

in another console

```
pi@raspberrypi:~ $ echo 864 > /dev/physmem
pi@raspberrypi:~ $ dmesg
[  869.551050]          Physical Address       Virtual Address
[  869.551065]            125C9003 (PGD)                 22000
[  869.551157]      E000000498FFC3 (PTE)                 22000
[  869.551180]      E8000007422F43 (PTE)                 23000
[  869.551197]                   0 (PTE)                 24000
[  869.551212]                   0 (PTE)                 25000
```

In the parent address space there is no change registered:


```
pi@raspberrypi:~ $ echo 863 > /dev/physmem
pi@raspberrypi:~ $ dmesg
[ 1092.845521]          Physical Address       Virtual Address
[ 1092.845537]            11323003 (PGD)                 22000
[ 1092.845552]      E000000498FFC3 (PTE)                 22000
[ 1092.845566]                   0 (PTE)                 23000
[ 1092.845581]                   0 (PTE)                 24000
[ 1092.845595]                   0 (PTE)                 25000
```

Congratulations, we just witnessed `copy-on-write` in action.

- Note that the addresses `F7XXX000` are in higher memory (4GB~), and that is
because the shared libraries are on those memory regions.

