
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

Disassembly of section .gnu.version_r:

00010428 <_init>:
   10428:	e92d4008 	push	{r3, lr}
   1042c:	eb000035 	bl	10508 <call_weak_fn>
   10430:	e8bd8008 	pop	{r3, pc}

Disassembly of section .plt:

00010434 <printf@plt-0x14>:
   10434:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
   10438:	e59fe004 	ldr	lr, [pc, #4]	; 10444 <_init+0x1c>
   1043c:	e08fe00e 	add	lr, pc, lr
   10440:	e5bef008 	ldr	pc, [lr, #8]!
   10444:	00010bbc 			; <UNDEFINED> instruction: 0x00010bbc

00010448 <printf@plt>:
   10448:	e28fc600 	add	ip, pc, #0, 12
   1044c:	e28cca10 	add	ip, ip, #16, 20	; 0x10000
   10450:	e5bcfbbc 	ldr	pc, [ip, #3004]!	; 0xbbc

00010454 <getchar@plt>:
   10454:	e28fc600 	add	ip, pc, #0, 12
   10458:	e28cca10 	add	ip, ip, #16, 20	; 0x10000
   1045c:	e5bcfbb4 	ldr	pc, [ip, #2996]!	; 0xbb4

00010460 <__stack_chk_fail@plt>:
   10460:	e28fc600 	add	ip, pc, #0, 12
   10464:	e28cca10 	add	ip, ip, #16, 20	; 0x10000
   10468:	e5bcfbac 	ldr	pc, [ip, #2988]!	; 0xbac

0001046c <wait@plt>:
   1046c:	e28fc600 	add	ip, pc, #0, 12
   10470:	e28cca10 	add	ip, ip, #16, 20	; 0x10000
   10474:	e5bcfba4 	ldr	pc, [ip, #2980]!	; 0xba4

00010478 <perror@plt>:
   10478:	e28fc600 	add	ip, pc, #0, 12
   1047c:	e28cca10 	add	ip, ip, #16, 20	; 0x10000
   10480:	e5bcfb9c 	ldr	pc, [ip, #2972]!	; 0xb9c

00010484 <__libc_start_main@plt>:
   10484:	e28fc600 	add	ip, pc, #0, 12
   10488:	e28cca10 	add	ip, ip, #16, 20	; 0x10000
   1048c:	e5bcfb94 	ldr	pc, [ip, #2964]!	; 0xb94

00010490 <__gmon_start__@plt>:
   10490:	e28fc600 	add	ip, pc, #0, 12
   10494:	e28cca10 	add	ip, ip, #16, 20	; 0x10000
   10498:	e5bcfb8c 	ldr	pc, [ip, #2956]!	; 0xb8c

0001049c <getpid@plt>:
   1049c:	e28fc600 	add	ip, pc, #0, 12
   104a0:	e28cca10 	add	ip, ip, #16, 20	; 0x10000
   104a4:	e5bcfb84 	ldr	pc, [ip, #2948]!	; 0xb84

000104a8 <exit@plt>:
   104a8:	e28fc600 	add	ip, pc, #0, 12
   104ac:	e28cca10 	add	ip, ip, #16, 20	; 0x10000
   104b0:	e5bcfb7c 	ldr	pc, [ip, #2940]!	; 0xb7c

000104b4 <fork@plt>:
   104b4:	e28fc600 	add	ip, pc, #0, 12
   104b8:	e28cca10 	add	ip, ip, #16, 20	; 0x10000
   104bc:	e5bcfb74 	ldr	pc, [ip, #2932]!	; 0xb74

000104c0 <abort@plt>:
   104c0:	e28fc600 	add	ip, pc, #0, 12
   104c4:	e28cca10 	add	ip, ip, #16, 20	; 0x10000
   104c8:	e5bcfb6c 	ldr	pc, [ip, #2924]!	; 0xb6c

Disassembly of section .bss:

00021048 <__stack_chk_guard@@GLIBC_2.4>:
   21048:       00000000        andeq   r0, r0, r0

0002104c <completed.9905>:
   2104c:       00000000        andeq   r0, r0, r0

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

Disassembly of section .dynamic:

00020f10 <_DYNAMIC>:
   20f10:	00000001 	andeq	r0, r0, r1
   20f14:	00000001 	andeq	r0, r0, r1
   20f18:	00000001 	andeq	r0, r0, r1
   20f1c:	00000060 	andeq	r0, r0, r0, rrx
   20f20:	0000000c 	andeq	r0, r0, ip
   20f24:	00010428 	andeq	r0, r1, r8, lsr #8
   20f28:	0000000d 	andeq	r0, r0, sp
   20f2c:	00010778 	andeq	r0, r1, r8, ror r7
   20f30:	00000019 	andeq	r0, r0, r9, lsl r0
   20f34:	00020f04 	andeq	r0, r2, r4, lsl #30
   20f38:	0000001b 	andeq	r0, r0, fp, lsl r0
   20f3c:	00000004 	andeq	r0, r0, r4
   20f40:	0000001a 	andeq	r0, r0, sl, lsl r0
   20f44:	00020f08 	andeq	r0, r2, r8, lsl #30
   20f48:	0000001c 	andeq	r0, r0, ip, lsl r0

Disassembly of section .data:

0002103c <__data_start>:
   2103c:	00000000 	andeq	r0, r0, r0
```

## Compile and deploy the driver

```shell
# Clean the module
$ perl ~/linux/scripts/checkpatch.pl -f phys_mem_per_proc.c
$ make
$ make deploy

# Install driver
$ ssh pi@192.168.2.2
pi@raspberrypi:~ $ sudo insmod phys_mem_per_proc.ko 
pi@raspberrypi:~ $ sudo chmod 666 /dev/physmem 
```

# Execute the program and print the me


Execute the program and don't type anything:

```shell
pi@raspberrypi:~ $ ./fork_userspace 
parent: Parent PID 994, Child PID 995
child: Child, PID 995
```

In another console print the memory for the parent and child and verify that
`copy-on-write` hasn't been done:

```shell
pi@raspberrypi:~ $ echo 994 > /dev/physmem 
pi@raspberrypi:~ $ echo 995 > /dev/physmem 
pi@raspberrypi:~ $ dmesg
```

## Parent Memory

```
[ 2381.177281] phys_mem_per_proc: loading out-of-tree module taints kernel.
[ 2381.177962] misc physmem: init() succeedded in registering device physmem
[ 2531.719800] misc physmem: Page information for PID [994]
[ 2531.719831] misc physmem:  # vmas         vma(ptr)        start          end       length(Hex=Decimal=KB)     RWESH
[ 2531.719860] misc physmem:      1:         c68c3bdd        10000        11000       1000 =    4096 =      4KB  R-E--
[ 2531.719885] misc physmem:      2:         16a5aed5        20000        21000       1000 =    4096 =      4KB  R----
[ 2531.719908] misc physmem:      3:         fd1499bf        21000        22000       1000 =    4096 =      4KB  RW---
[ 2531.719944] misc physmem:      4:         ab74b047      1AA2000      1AC3000      21000 =  135168 =    135KB  RW---
[ 2531.719969] misc physmem:      5:         7bbf5575     F7B2A000     F7C62000     138000 = 1277952 =   1277KB  R-E--
[ 2531.719993] misc physmem:      6:         2f09b9f4     F7C62000     F7C72000      10000 =   65536 =     65KB  -----
[ 2531.720099] misc physmem:      7:         ed4fa3a1     F7C72000     F7C74000       2000 =    8192 =      8KB  R----
[ 2531.720129] misc physmem:      8:         16aa35cf     F7C74000     F7C75000       1000 =    4096 =      4KB  RW---
[ 2531.720153] misc physmem:      9:         4ba7fe56     F7C75000     F7C78000       3000 =   12288 =     12KB  RW---
[ 2531.720178] misc physmem:     10:         feadf9f5     F7C8C000     F7C90000       4000 =   16384 =     16KB  R-E--
[ 2531.720202] misc physmem:     11:         e3521f22     F7C90000     F7C9F000       F000 =   61440 =     61KB  -----
[ 2531.720226] misc physmem:     12:         62be89ad     F7C9F000     F7CA0000       1000 =    4096 =      4KB  R----
[ 2531.720267] misc physmem:     13:         763b248c     F7CA0000     F7CA1000       1000 =    4096 =      4KB  RW---
[ 2531.720291] misc physmem:     14:         2810d81f     F7CA1000     F7CC1000      20000 =  131072 =    131KB  R-E--
[ 2531.720315] misc physmem:     15:          f35f66d     F7CCD000     F7CCF000       2000 =    8192 =      8KB  RW---
[ 2531.720339] misc physmem:     16:         f5d23ed3     F7CD0000     F7CD1000       1000 =    4096 =      4KB  R-E--
[ 2531.720363] misc physmem:     17:          d4d8e75     F7CD1000     F7CD2000       1000 =    4096 =      4KB  R----
[ 2531.720387] misc physmem:     18:         33517b87     F7CD2000     F7CD3000       1000 =    4096 =      4KB  RW---
[ 2531.720411] misc physmem:     19:         c7389f30     FFD03000     FFD24000      21000 =  135168 =    135KB  RW---
[ 2531.720447] misc physmem:     20:         18c385ff     FFFF0000     FFFF1000       1000 =    4096 =      4KB  R-E--
[ 2531.720464] misc physmem:  
[ 2531.720481] misc physmem:          Physical Address       Virtual Address
[ 2531.720505] misc physmem:            155DC003 (PGD)                 10000
[ 2531.720526] misc physmem:      20000015CFDFC3 (PTE)                 10000
[ 2531.720543] misc physmem:  
[ 2531.720560] misc physmem:          Physical Address       Virtual Address
[ 2531.720579] misc physmem:            155DC003 (PGD)                 20000
[ 2531.720599] misc physmem:      E0000015D7FFC3 (PTE)                 20000
[ 2531.720616] misc physmem:  
[ 2531.720633] misc physmem:          Physical Address       Virtual Address
[ 2531.720652] misc physmem:            155DC003 (PGD)                 21000
[ 2531.720671] misc physmem:      E8000015E2AF43 (PTE)                 21000
[ 2531.720688] misc physmem:  
[ 2531.720704] misc physmem:          Physical Address       Virtual Address
[ 2531.720723] misc physmem:            155DC003 (PGD)               1AA2000
[ 2531.720743] misc physmem:      E8000015D7DF43 (PTE)               1AA2000
```

## Child memory

```
[ 2537.276597] misc physmem: Page information for PID [995]
[ 2537.276628] misc physmem:  # vmas         vma(ptr)        start          end       length(Hex=Decimal=KB)     RWESH
[ 2537.276656] misc physmem:      1:         d15dc113        10000        11000       1000 =    4096 =      4KB  R-E--
[ 2537.276680] misc physmem:      2:         b61b567e        20000        21000       1000 =    4096 =      4KB  R----
[ 2537.276704] misc physmem:      3:         82f6bd47        21000        22000       1000 =    4096 =      4KB  RW---
[ 2537.276729] misc physmem:      4:         706801d0      1AA2000      1AC3000      21000 =  135168 =    135KB  RW---
[ 2537.276754] misc physmem:      5:         91003647     F7B2A000     F7C62000     138000 = 1277952 =   1277KB  R-E--
[ 2537.276778] misc physmem:      6:         8a36ab8b     F7C62000     F7C72000      10000 =   65536 =     65KB  -----
[ 2537.276814] misc physmem:      7:         5de9b326     F7C72000     F7C74000       2000 =    8192 =      8KB  R----
[ 2537.276838] misc physmem:      8:         4c4b225f     F7C74000     F7C75000       1000 =    4096 =      4KB  RW---
[ 2537.276862] misc physmem:      9:         cdd369ec     F7C75000     F7C78000       3000 =   12288 =     12KB  RW---
[ 2537.276886] misc physmem:     10:         fa879c86     F7C8C000     F7C90000       4000 =   16384 =     16KB  R-E--
[ 2537.276909] misc physmem:     11:         2e4ebe70     F7C90000     F7C9F000       F000 =   61440 =     61KB  -----
[ 2537.276933] misc physmem:     12:         93896eab     F7C9F000     F7CA0000       1000 =    4096 =      4KB  R----
[ 2537.276957] misc physmem:     13:         9583cfd2     F7CA0000     F7CA1000       1000 =    4096 =      4KB  RW---
[ 2537.276981] misc physmem:     14:         eb4f50bc     F7CA1000     F7CC1000      20000 =  131072 =    131KB  R-E--
[ 2537.277102] misc physmem:     15:         1497bb1e     F7CCD000     F7CCF000       2000 =    8192 =      8KB  RW---
[ 2537.277126] misc physmem:     16:         a7929294     F7CD0000     F7CD1000       1000 =    4096 =      4KB  R-E--
[ 2537.277150] misc physmem:     17:         e286f593     F7CD1000     F7CD2000       1000 =    4096 =      4KB  R----
[ 2537.277174] misc physmem:     18:         1b384602     F7CD2000     F7CD3000       1000 =    4096 =      4KB  RW---
[ 2537.277200] misc physmem:     19:         f2d3df95     FFD03000     FFD24000      21000 =  135168 =    135KB  RW---
[ 2537.277241] misc physmem:     20:         5a5c4e04     FFFF0000     FFFF1000       1000 =    4096 =      4KB  R-E--
[ 2537.277258] misc physmem:  
[ 2537.277275] misc physmem:          Physical Address       Virtual Address
[ 2537.277300] misc physmem:            155F0003 (PGD)                 10000
[ 2537.277324] misc physmem:      20000015CFDFC3 (PTE)                 10000
[ 2537.277341] misc physmem:  
[ 2537.277359] misc physmem:          Physical Address       Virtual Address
[ 2537.277378] misc physmem:            155F0003 (PGD)                 20000
[ 2537.277398] misc physmem:      E0000015D7FFC3 (PTE)                 20000
[ 2537.277415] misc physmem:  
[ 2537.277432] misc physmem:          Physical Address       Virtual Address
[ 2537.277451] misc physmem:            155F0003 (PGD)                 21000
[ 2537.277470] misc physmem:      E800001266CF43 (PTE)                 21000
```


## Important things to `note`

- The first 2 pages for the parent and child point to the same physical page tables:

```
20000015CFDFC3 (PTE)                 10000
E0000015D7FFC3 (PTE)                 20000
```

- The third page in the parent and child point to different physical page tables:

```
# Parent memory
[ 2531.720633] misc physmem:          Physical Address       Virtual Address
[ 2531.720652] misc physmem:            155DC003 (PGD)                 21000
[ 2531.720671] misc physmem:      E8000015E2AF43 (PTE)                 21000

# Child Memory
[ 2537.277432] misc physmem:          Physical Address       Virtual Address
[ 2537.277451] misc physmem:            155F0003 (PGD)                 21000
[ 2537.277470] misc physmem:      E800001266CF43 (PTE)                 21000
```
If we check the objdump, we will see that the stack is located in the virtual
page `21000`. Remember that when `fork()` is executed, parent and child has
different stack.

- Note that the addresses `F7XXX000` are in higher memory (4GB~), and that is
because the shared libraries are on those memory regions.



