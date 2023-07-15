
# Chapter 0 - Getting Started

In this chapter we will show how to get started with the `Linux Kernel` using
`qemu`, `builtroot` and `gcc-aarch64-linux-gnu` cross compiler.


## Install libraries and cross compilers

### Install tools and libraries

```shell
 $ sudo apt install bc bison flex libssl-dev make libc6-dev libncurses-dev

 # Tool to create EXT2 filesystems.
 $ sudo apt install genext2fs
```

### Install aarch64 cross compiler

```shell
 $ sudo apt install gcc-aarch64-linux-gnu
```

### Install qemu

```shell
 # Install qemu with ARM support
 $ sudo apt install qemu-system

 # Install qemu with mips support
 $ sudo apt install qemu-system-mips

 # Install all the architectures supported by qemu.
 $ sudo apt install qemu-system
```

## How to get the `Linux Kernel`, configure and build it


### Get Linux Kernel

```shell
 $ git clone git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
 $ cd linux-stable/

 # See the stable branches available.
 $ git branch -va
 $ git checkout -b linux-6.3.y origin/linux-6.3.y
 d1047d75f77a (HEAD -> linux-6.3.y, tag: v6.3.13, origin/linux-6.3.y) Linux 6.3.13
```

### Ccnfigure and Build

```shell
 # Configure
 $ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- defconfig
 $ vim .config

 # Build
 $ make -j40 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
  LD      vmlinux
  NM      System.map
  SORTTAB vmlinux
  OBJCOPY arch/arm64/boot/Image
  GZIP    arch/arm64/boot/Image.gz

 # Clean configuration and binaries
 $ make -j40 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- mrproper
```

## How to get the `root filesystem`

We can get the latest stable `buildroot` code from:

https://buildroot.uclibc.org/download.html

We can also use `git` to check out the code.

```shell
 $ git clone git://git.buildroot.net/buildroot
 # cd buildroot
 $ git log --oneline
 3c66f65a6a (HEAD -> master, origin/master, origin/HEAD) package/libopenssl: bump version to 3.0.9
 d3db2e61ce .checkpackageignore: remove entry to tovid patch

 $ git branch -va
 remotes/origin/2023.05.x c32b8b57eb {linux, linux-headers}: bump 4.{14, 19}.x / 5.{4, 10, 15}.x / 6.{1, 3}.x series
```

After we got the src, we can configure `buildroot` with the options below:

```shell

 # Select the options in the buildroot menu
 $ make menuconfig

    Target options
         Target Architecture (AArch64 (little endian))
         Target Architecture Variant (cortex-A73)

    Toolchain type (External toolchain)
         *** Toolchain External Options ***
         Toolchain (Linaro AArch64 2018.05)
         Toolchain origin (Toolchain to be downloaded and installed)

    System Configuration
         [*] Enable root login with password (NEW)
             ()    Root password (NEW)
         /bin/sh (busybox' default shell)
         [*] Run a getty (login prompt) after boot

    Target packages
         [*]   Show packages that are also provided by busybox

         Development tools
             [*] grep
             [*] sed
             [*] tree

         Networking applications
             [*] dhcpcd
             [*] iproute2
             [*] openssh

         Shell and Utilities
             [*] file
             [*] time

         Text editors and viewers
             [*] vim

    Filesystem images
         [*] ext2/3/4 root filesystem
             ext2/3/4 variant (ext4)
         [*] tar the root filesystem

 # Build build root.
 $ make

 # Images are generated in:
 $ ls output/images
rootfs.ext2  rootfs.ext4  rootfs.tar
```

## How to modify the root filesystem

To modify the root filesystem that `buildroot` created, follow the instructions
in `buildroot/output/target/THIS_IS_NOT_YOUR_ROOT_FILESYSTEM`

```shell
 $ sudo tar -C /destination/of/extraction -xf output/images/rootfs.tar

 # Modify the files
 $ cd /destination/of/extraction
 $ ls -la etc/profile
-rw-r--r-- 1 root root 312 Jul 15 08:41 etc/profile
 $ sudo vim etc/profile
```

### Generate Ext2 filesystem

```shell
 $ sudo genext2fs -b 65536 -d /<path to rootfs> -U newrootfs.ext2
 $ file newrootfs.ext2
 newrootfs.ext2: Linux rev 0.0 ext2 filesystem data, UUID=00000000-0000-0000-0000-000000000000
```

### Generate a Ext4 filesystem

```shell
 $ sudo mkfs.ext4 -d ./newrootfs -r 1 -N 0 -m 5 -L "rootfs" -I 256 -O ^64bit ./newrootfs.ext4 "100M"
 mke2fs 1.45.5 (07-Jan-2020)
 64-bit filesystem support is not enabled.  The larger fields afforded by this
 feature enable full-strength checksumming.  Pass -O   64bit to rectify.
 Discarding device blocks: done
 Creating filesystem with 25600 4k blocks and 25600 inodes

 Allocating group tables: done
 Writing inode tables: done
 Creating journal (1024 blocks): done
 Copying files into the device: done
 Writing superblocks and filesystem accounting information: done

 $ file newrootfs.ext4
 newrootfs.ext4: Linux rev 1.0 ext4 filesystem data, UUID=756ef8e1-04a8-4c6e-9d34-3e401d9f56ee,
    volume name "rootfs" (extents)  (large files) (huge files)
```

## How to start `qemu`

```shell
 $ sudo qemu-system-aarch64 \
     -m 2048 \
     -cpu cortex-a73 \
     -machine virt \
     -nographic -smp 1 \
     -hda <path to rootfs>/output/images/rootfs.ext2 \
     -kernel <path to kernel>/linux-stable/arch/arm64/boot/Image \
     -append "console=ttyAMA0 root=/dev/vda oops=panic panic_on_warn=1 panic=-1 debug earlyprintk=serial"

 $ uname -a
 Linux buildroot 6.3.13 #1 SMP PREEMPT Sat Jul 15 12:59:22 PDT 2023 aarch64 GNU/Linux

 $ mount
 /dev/root on / type ext4 (rw,relatime)
 devtmpfs on /dev type devtmpfs (rw,relatime,size=992020k,nr_inodes=248005,mode=755)
 proc on /proc type proc (rw,relatime)
 devpts on /dev/pts type devpts (rw,relatime,gid=5,mode=620,ptmxmode=666)
 tmpfs on /dev/shm type tmpfs (rw,relatime,mode=777)
 tmpfs on /tmp type tmpfs (rw,relatime)
 tmpfs on /run type tmpfs (rw,nosuid,nodev,relatime,mode=755)
 sysfs on /sys type sysfs (rw,relatime)

 $ df -h
 Filesystem                Size      Used Available Use% Mounted on
 /dev/root                63.7M     43.7M     16.8M  72% /
 devtmpfs                968.8M         0    968.8M   0% /dev
 tmpfs                   988.8M         0    988.8M   0% /dev/shm
 tmpfs                   988.8M     36.0K    988.8M   0% /tmp
 tmpfs                   988.8M     24.0K    988.8M   0% /run
```

To leave qemu, execute `Ctrl + Alt and x`.
