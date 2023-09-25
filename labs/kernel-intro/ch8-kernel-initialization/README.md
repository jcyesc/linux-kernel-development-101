
# Chapter 8 - Kernel initialization

The docs that explains the booting process in ARM64 are found in:

- linux/Documentation/arm64/booting.rst


## Kernel header

The header of a linux kernel image for ARM64 contains a 64-bit header with this
information:

```shell
  u32 code0;			/* Executable code */
  u32 code1;			/* Executable code */
  u64 text_offset;		/* Image load offset, little endian */
  u64 image_size;		/* Effective Image size, little endian */
  u64 flags;			/* kernel flags, little endian */
  u64 res2	= 0;		/* reserved */
  u64 res3	= 0;		/* reserved */
  u64 res4	= 0;		/* reserved */
  u32 magic	= 0x644d5241;	/* Magic number, little endian, "ARM\x64" */
  u32 res5;			/* reserved (used for PE COFF offset) */
```

We can see the values using:

```shell
 $ hexdump -C -n 64 ./arch/arm64/boot/Image
00000000  4d 5a 40 fa 27 b8 65 14  00 00 00 00 00 00 00 00  |MZ@.'.e.........|
00000010  00 00 65 02 00 00 00 00  0a 00 00 00 00 00 00 00  |..e.............|
00000020  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
00000030  00 00 00 00 00 00 00 00  41 52 4d 64 40 00 00 00  |........ARMd@...|
```

## Memory layout after booting

As per `Documentation/arm64/memory.rst`, the AArch64 Linux memory layout with
4KB pages and 4 levels (48-bit) looks like this:

```
  Start			End			Size		Use
  -----------------------------------------------------------------------
  0000000000000000	0000ffffffffffff	 256TB		user
  ffff000000000000	ffff7fffffffffff	 128TB		kernel logical memory map
  ffff800000000000	ffff9fffffffffff	  32TB		kasan shadow region
  ffffa00000000000	ffffa00007ffffff	 128MB		bpf jit region
  ffffa00008000000	ffffa0000fffffff	 128MB		modules
  ffffa00010000000	fffffdffbffeffff	 ~93TB		vmalloc
  fffffdffbfff0000	fffffdfffe5f8fff	~998MB		[guard region]
  fffffdfffe5f9000	fffffdfffe9fffff	4124KB		fixed mappings
  fffffdfffea00000	fffffdfffebfffff	   2MB		[guard region]
  fffffdfffec00000	fffffdffffbfffff	  16MB		PCI I/O space
  fffffdffffc00000	fffffdffffdfffff	   2MB		[guard region]
  fffffdffffe00000	ffffffffffdfffff	   2TB		vmemmap
  ffffffffffe00000	ffffffffffffffff	   2MB		[guard region]
```
