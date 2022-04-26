# Linux Kernel Internals and Development Version 5.14 (LFD420)


## Structure of the repo

The structure of the repo contains these 2 main directories

```
├── labs
│   └── kernel-intro
│   └── drivers
├── linux
└── README.md
```

- labs/kernel-intro: contains an intro to the Linux Kernel development.
- labs/drivers: contains an intro to the Linux Driver development.
- linux: contains the Linux source code for the Raspberry Pi.


## Steps to install Raspbian

For detailed instructions see:

https://www.raspberrypi.com/software/

The first step is to install Raspbian in the SD card. To do
that, we will use `Raspberry Pi OS`.

> Note: Insert the SD card in your computer.

```shell
$ sudo apt install rpi-imager
# Select the OS and flash the image to the SD card.
$ rpi-imager
```

## Configure the Network interface in Raspberry Pi and Mac OS

On the Raspberry Pi board (the target), configure the eth0 interface with the
IP address 192.168.2.2 by editing the `/etc/network/interfaces` file.

```shell
sudo nano /etc/network/interfaces

auto eth0
iface eth0 inet static
   address 192.168.2.2
   netmask 255.255.255.0
```

Configure ssh to start during booting time:

```shell
sudo nano /etc/rc.local

/etc/init.d/ssh start

exit 0
```

Restart the Raspberry Pi.

In the Mac, go to `System Preferences > Sharing` and select Internet Sharing and in the right select `USB 10/100/100 LAN`.

To connect to the Raspberry Pi using SSH, run the following command:

```shell
ssh pi@192.168.2.2
```

To transfer a file from the host to the target, execute:

```
scp my_file pi@192.168.2.2:~
```

## Cross-compile and install the `64-bit` Linux kernel

After you have installed Raspbian in the SD card using `Raspberry Pi OS`, you might
want to compile and install newer versions of the Kernel. This section explains
how to do it.

For detailed instructions see:

https://www.raspberrypi.com/documentation/computers/linux_kernel.html

Instal the tooling necessary to get and compile the kernel.

```shell
sudo apt install git bc bison flex libssl-dev make libc6-dev libncurses5-dev
sudo apt install crossbuild-essential-arm64
sudo apt install libncurses5-dev
```

Get the linux kernel:

> Note: This step might be skipped because the linux kernel is already
> under the `linux` directory.

```shell
$ git clone --depth=1 https://github.com/raspberrypi/linux
$ cd linux
$ git branch
* rpi-5.10.y
$ git log --oneline
6cfe1a8 staging/bcm2835-camera: Add support for MPEG_VIDEO_FORCE_KEY_FRAME
```

Compile the `64-bit` Linux kernel, modules and device trees:

```
$ KERNEL=kernel8
# Always install the default configuration
$ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- bcmrpi3_defconfig

# In case you want to configure a particular feature, use menuconfig.
# https://www.raspberrypi.com/documentation/computers/linux_kernel.html#preparing-to-configure
$ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- menuconfig
$ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- Image -j 8 modules dtbs
```

Install the image in the SD card:

```shell
$ lsblk
NAME   MAJ:MIN RM  SIZE RO TYPE MOUNTPOINT
sr0     11:0    1 1024M  0 rom
sdc      8:32   1 14.9G  0 disk
├─sdc2   8:34   1 14.6G  0 part /media/parallels/rootfs
└─sdc1   8:33   1  256M  0 part /media/parallels/boot

$ mkdir mnt
$ mkdir mnt/fat32
$ mkdir mnt/ext4
$ sudo mount /dev/sdc1 mnt/fat32
$ sudo mount /dev/sdc2 mnt/ext4
$ sudo env PATH=$PATH make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
     INSTALL_MOD_PATH=mnt/ext4 modules_install

$ sudo cp mnt/fat32/$KERNEL.img mnt/fat32/$KERNEL-backup.img
$ sudo cp arch/arm64/boot/Image mnt/fat32/$KERNEL.img
$ sudo cp arch/arm64/boot/dts/broadcom/*.dtb mnt/fat32/
$ sudo cp arch/arm64/boot/dts/overlays/*.dtb* mnt/fat32/overlays/
$ sudo cp arch/arm64/boot/dts/overlays/README mnt/fat32/overlays/
$ sudo umount mnt/fat32
$ sudo umount mnt/ext4
```

## Kernel version and Kernel release

The following commands helps you to identify the kernel version that you are building
and the name of the kernel release:

```shell
$ make ARCH=arm64 kernelversion
5.10.75
$ make ARCH=arm64 kernelrelease
arch/arm64/Makefile:25: ld does not support --fix-cortex-a53-843419; kernel may be susceptible to erratum
5.10.75-v8+
```

## Find out if the OS is 32-bit or 64-bit

```shell
# Go to the linux kernel directory.
$ cd linux
$ file vmlinux
vmlinux: ELF 64-bit LSB shared object, ARM aarch64, version 1 (SYSV), statically linked, BuildID[sha1]=d5a8bf0bbb5a55d98492b8b9e3eda9f320d1a2f1, not stripped
$ file kernel/sys.o
kernel/sys.o: ELF 64-bit LSB relocatable, ARM aarch64, version 1 (SYSV), not stripped
```

## Find out if the user space is 32-bit or 64-bit

```shell
pi@raspberrypi:~ $ getconf LONG_BIT
32
pi@raspberrypi:~ $ file /sbin/init
/sbin/init: symbolic link to /lib/systemd/systemd
pi@raspberrypi:~ $ file /lib/systemd/systemd
/lib/systemd/systemd: ELF 32-bit LSB shared object, ARM, EABI5 version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux-armhf.so.3, for GNU/Linux 3.2.0, BuildID[sha1]=125fb1f7bd746a32b6d763ad9a91d30ddc03a96c, stripped
```


