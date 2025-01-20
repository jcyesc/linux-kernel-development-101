
# Getting Started

In this section we will show to get started with the `Linux Kernel` using
`qemu`, `builtroot` and `gcc-aarch64-linux-gnu` cross compiler. On top
of this, we'll show how to build and add a basic module to the rootfs.


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

### Configure and Build

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

 # Keep the .config file and remove binaries.
 $ make -j40 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- clean

 # Remove the .config file and remove binaries
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

 # Note: it is important that you pick a Linux kernel which headers are
 # compatible with the ones defined in Build root.
 $ git branch -va
 remotes/origin/2023.05.x c32b8b57eb {linux, linux-headers}: bump 4.{14, 19}.x / 5.{4, 10, 15}.x / 6.{1, 3}.x series
```

To check the linux headers supported in `buildroot`, run:

```shell
 $ git log --oneline | grep linux-headers | head -5
b8c92f7d48 {linux, linux-headers}: bump 4.{14, 19}.x / 5.{4, 10, 15}.x / 6.{1, 5, 6}.x series
4eb9c5d451 {linux, linux-headers}: bump 6.{1, 5}.x series
f3a81cc068 {toolchain, linux-headers}: add support for 6.6 headers
96298d3b54 {linux, linux-headers}: bump 4.{14, 19}.x / 5.{4, 10, 15}.x / 6.{1, 5}.x series
d8bcff69fc {linux, linux-headers}: bump 5.15.x / 6.{1, 5}.x series
```

After we got the src, we can configure `buildroot` with the options below:

> The file `buildroot_config` in this directory contains a default configuration.

```shell

 # Select the options in the buildroot menu
 $ make menuconfig

    Target options
         Target Architecture (AArch64 (little endian))
         Target Architecture Variant (cortex-A76)
         MMU Page Size (4KB)  --->

    Toolchain type (External toolchain)
         *** Toolchain External Options ***
         Toolchain (Arm AArch64 13.2.rel1)
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

         Filesystem and flash utilities
             -*- e2fsprogs  --->
             [*] e2tools
             [*] erofs-utils
             [*]   lz4 support
             [*]   erofsfuse support
             [*] f2fs-tools
             [*] genext2fs

         Networking applications
             [*] dhcpcd
             [*] iproute2
             [*] iptables
             [*] openssh

         Shell and Utilities
             [*] file
             [*] time

         Text editors and viewers
             [*] vim

    Filesystem images
         [*] erofs root filesystem
         [*] ext2/3/4 root filesystem
             ext2/3/4 variant (ext4)
         [*] tar the root filesystem
         (600M) exact size

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

### Generate a `ext4` filesystem

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

### Generate a `ext2` filesystem

```shell
 $ sudo genext2fs -b 65536 -d /<path to rootfs> -U newrootfs.ext2
 $ file newrootfs.ext2
 newrootfs.ext2: Linux rev 0.0 ext2 filesystem data, UUID=00000000-0000-0000-0000-000000000000
```

Note: If the disk is corrupted, we might fix it with:

```
 $ e2fsck -p ~/images/rootfs.ext2
```

## How to start `qemu`

```shell
 $ sudo qemu-system-aarch64 \
     -m 2048 \
     -cpu cortex-a72 \
     -machine virt \
     -nographic -smp 1 \
     -hda <path to rootfs>/output/images/rootfs.ext2 \
     -kernel <path to kernel>/linux-stable/arch/arm64/boot/Image \
     -append "console=ttyAMA0 root=/dev/vda oops=panic panic_on_warn=1 panic=-1 debug earlyprintk=serial" \
     -object rng-random,filename=/dev/urandom,id=rng0 \
     -device virtio-rng-pci,rng=rng0 \
     -device virtio-net-pci,netdev=eth0 \
     -netdev user,id=eth0,hostfwd=tcp::8022-:22

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

`Note:` that `/dev/vda`  and `/` refers to the same filesystem `/dev/root`:

```shell
 $ cat /etc/fstab
# <file system>	<mount pt>	<type>	<options>	<dump>	<pass>
/dev/root	/		ext2	rw,noauto	0	1
proc		/proc		proc	defaults	0	0
devpts		/dev/pts	devpts	defaults,gid=5,mode=620,ptmxmode=0666	0	0
tmpfs		/dev/shm	tmpfs	mode=0777	0	0
tmpfs		/tmp		tmpfs	mode=1777	0	0
tmpfs		/run		tmpfs	mode=0755,nosuid,nodev	0	0
sysfs		/sys		sysfs	defaults	0	0

 $ df /dev/vda
Filesystem           1K-blocks      Used Available Use% Mounted on
/dev/root                91864     45272     39424  53% /

root $ df /
Filesystem           1K-blocks      Used Available Use% Mounted on
/dev/root                91864     45272     39424  53% /
```

The configuraton files can be found in `/etc`.

```shell
 $ ls /etc/
dhcpcd.conf    init.d         network        profile.d      shells
fstab          inittab        nsswitch.conf  protocols      ssh
group          inputrc        os-release     resolv.conf    ssl
group-         iproute2       passwd         services
hostname       issue          passwd-        shadow
hosts          mtab           profile        shadow-

 $ ls /etc/init.d/
S01syslogd  S02sysctl   S40network  S50sshd     rcS
S02klogd    S20seedrng  S41dhcpcd   rcK
```

After the kernel boots, `/sbin/init` is called and it starts executing the
scripts defined in:

- /etc/inittab
- /etc/init.d/

To leave qemu, execute `Ctrl + Alt and x`.


## How to add network support

To configure the network in qemu, add these 2 lines to `qemu-system-aarch64`:

```shell
host $ sudo qemu-system-aarch64

        ...

     -object rng-random,filename=/dev/urandom,id=rng0 \
     -device virtio-rng-pci,rng=rng0 \
     -device virtio-net-pci,netdev=eth0 \
     -netdev user,id=eth0,hostfwd=tcp::8022-:22
```

Generate a public ssh key in the host:

```shell
host $  ssh-keygen
host $  ls ~/.ssh/
id_rsa.pub

# Copy the content of id_rsa.pub
host $ cat ~/.ssh/id_rsa.pub
```

Paste the content of `id_rsa.pub` in `authorized_keys`.

```shell
# In qemu, execute
root $ mkdir ~/.ssh

# Paste the content of id_rsa.pub
root $ vim .ssh/authorized_keys
```

Configure the `eth0` interface in `/etc/network/interfaces`.

```shell
# cat /etc/network/interfaces
# interface file auto-generated by buildroot

auto lo
iface lo inet loopback

auto eth0
iface eth0 inet static
	address 127.0.0.1
	netmask 255.255.255.0
```

Connect to qemu using ssh:

```shell
host $ ssh root@127.0.0.1 -p 8022

root $
```

To upload files via `scp` use:

```shell
host $ scp -P 8022 libshared.so  root@127.0.0.1:~
```

### Debugging Network issues

After configuring the network and ssh, you might have some troubles connecting
to qemu. There are many things that could go wrong. One of the most common
is to pass the wrong CPU to qemu.

An indicator of this issue is the message `Starting sshd: Illegal instruction`
in the kernel logs.

```
Illegal instruction
Starting sshd: Illegal instruction
```

To fix the issue, make sure that the qemu parameter `-cpu` matches what it was
configured in `buildroot`. If the buildroot configuration is using
`BR2-cortex_a76=y`, then qemu has to use `-cpu cortex-a76` as parameter:

```
 $ cd buildroot
 $ cat .config | grep cortex
BR2_GCC_TARGET_CPU="cortex-a9"
# BR2_cortex_a5 is not set
# BR2_cortex_a7 is not set
# BR2_cortex_a8 is not set
# BR2_cortex_a9 is not set
# BR2_cortex_a12 is not set
# BR2_cortex_a15 is not set
# BR2_cortex_a15_a7 is not set
# BR2_cortex_a17 is not set
# BR2_cortex_a17_a7 is not set
# BR2_cortex_m3 is not set
# BR2_cortex_m4 is not set
# BR2_cortex_m7 is not set
# BR2_cortex_a32 is not set
# BR2_cortex_a35 is not set
# BR2_cortex_a53 is not set
# BR2_cortex_a57 is not set
# BR2_cortex_a57_a53 is not set
# BR2_cortex_a72 is not set
# BR2_cortex_a72_a53 is not set
# BR2_cortex_a73 is not set
# BR2_cortex_a73_a35 is not set
# BR2_cortex_a73_a53 is not set
BR2_cortex_a76=y
# BR2_cortex_a76_a55 is not set
```

After passing the right parameter to qemu, in this case `-cpu cortex-a76`,
we should see in the logs that `sshd` started successfully.

```
eth0: adding default route via 10.0.2.2
forked to background, child pid 99
[   13.186904] audit: type=1326 audit(1710616597.236:2): auid=4294967295 uid=100 gid=101 ses=4294967295 pid=99 comm="dhcpcd" exe="/sbin/dhcpcd" sig=31 arch=c00000b7 syscall=24 compat=0 ip=0xffffa53d928c code=0x0
dhcpcd_fork_cb: truncated read 0 (expected 4)
Starting sshd: OK
```

## How to build and add a module to the rootfs

Once that you have your module, it is necessary to compile it using the kernel
where it is going to be deployed. To compile the module, run the command below
where the module is located:

```shell
 $ KERNEL_DIR=$(HOME)/linux-stable \
     make -C $(KERNEL_DIR) \
         ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
         M=$(PWD) modules
 $ ls
hello_world_mod.c   hello_world_mod.mod    hello_world_mod.mod.o
hello_world_mod.ko  hello_world_mod.mod.c  hello_world_mod.o
Module.symvers Makefile modules.order
```

Once that is compiled, we can include it in the rootfs.

```shell
 $ sudo tar -C /destination/of/extraction -xf output/images/rootfs.tar
 $ mkdir -p /destination/of/extraction/lib/modules
 $ cp hello_world_mod.ko /destination/of/extraction/lib/modules

 # Create the rootfs (/destination/of/extraction is newrootfs dir).
 $ sudo mkfs.ext4 -d ./newrootfs -r 1 -N 0 -m 5 -L "rootfs" \
                     -I 256 -O ^64bit ./newrootfs.ext4 "100M"
```

Start qemu with the new rootfs:

```shell
 $ hda=<path to rootfs/newrootfs.ext4>
 $ kernel=<path kernel/linux-stable/arch/arm64/boot/Image>
 $ sudo qemu-system-aarch64 -m 2048 -cpu cortex-a72 -machine virt \
       -nographic -smp 1  -hda $hda \
       -kernel $kernel  \
       -append "console=ttyAMA0 root=/dev/vda oops=panic panic_on_warn=1 panic=-1 debug earlyprintk=serial" \
       -object rng-random,filename=/dev/urandom,id=rng0 \
       -device virtio-rng-pci,rng=rng0 \
       -device virtio-net-pci,netdev=eth0 \
       -netdev user,id=eth0,hostfwd=tcp::8022-:22
```

Load the module

```shell
root $ insmod /lib/modules/hello_world_mod.ko
[   16.598020] hello_world_mod: loading out-of-tree module taints kernel.
[   16.607746] hello_world_mod: hello_init() Init Hello World!

root $ lsmod
Module                  Size  Used by    Tainted: G
hello_world_mod        16384  0

root $ rmmod hello_world_mod.ko
[  196.477130] hello_world_mod: hello_exit() Exit Hello World!

root $ lsmod
Module                  Size  Used by    Tainted: G
```
