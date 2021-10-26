# Linux Kernel Internals and Development Version 5.14 (LFD420)

This repo contains the labs for the LFD420 training. It uses
the `Raspberry Pi 3 B+` and `Raspbian` to perform the exercises.

## Structure of the repo

The structure of the repo contains these 2 main directories

```
.
├── labs
├── linux
└── README.md
```

- labs: contains the solution to the labs.
- linux: contains the Linux source code the Raspberry Pi.


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

## Cross-compile and install the Linux kernel

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

Compile the Linux kernel, modules and device trees:

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
sdb
   sdb1
   sdb2

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

## Find out if the OS is 32-bit or 64-bit

```shell
pi@raspberrypi:~ $ getconf LONG_BIT
32
pi@raspberrypi:~ $ file /sbin/init
/sbin/init: symbolic link to /lib/systemd/systemd
pi@raspberrypi:~ $ file /lib/systemd/systemd
/lib/systemd/systemd: ELF 32-bit LSB shared object, ARM, EABI5 version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux-armhf.so.3, for GNU/Linux 3.2.0, BuildID[sha1]=125fb1f7bd746a32b6d763ad9a91d30ddc03a96c, stripped
```


