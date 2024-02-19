
# Basic File System

The `basic_fs.ko` module implements a file system that does not need a block
device to be mounted. To register the file system, first mount the moduele:

```
$ uname -a
Linux buildroot 6.6.14 #30 SMP PREEMPT Sat Jan 27 22:59:17 PST 2024 aarch64 GNU/Linux

# Register filesystem
$ insmod basic_fs.ko
$ cat /proc/filesystems | grep basicfs
nodev	basicfs
```

The next step is to mount the `basicfs` file system:

```
$ mkdir -p /mnt/basic

# Mounting the filesystem
$ mount -t basicfs none /mnt/basic
$ mount
/dev/root on / type ext4 (rw,relatime)
devtmpfs on /dev type devtmpfs (rw,relatime,size=995008k,nr_inodes=248752,mode=755)
proc on /proc type proc (rw,relatime)
devpts on /dev/pts type devpts (rw,relatime,gid=5,mode=620,ptmxmode=666)
tmpfs on /dev/shm type tmpfs (rw,relatime,mode=777)
tmpfs on /tmp type tmpfs (rw,relatime)
tmpfs on /run type tmpfs (rw,nosuid,nodev,relatime,mode=755)
sysfs on /sys type sysfs (rw,relatime)
none on /mnt/basic type basicfs (rw,relatime)
```

Finally, create some files and directories in the file system and check their
inodes.

```
$ echo "Creating a file" > /mnt/basic/regfile.txt
$ echo "Creating a file 2" > /mnt/basic/regfile2.txt
$ mkdir /mnt/basic/private

$ ls -i /mnt/
    703 basic
$ ls -i /mnt/basic/
    706 private           704 regfile.txt       705 regfile2.txt
# ln -s /mnt/basic/regfile.txt /mnt/basic/regfile-link.txt
# ls -li /mnt/basic/
total 8
    710 drwxr-xr-x    2 root     root             0 Feb 19 06:51 private
    715 lrwxrwxrwx    1 root     root            22 Feb 19 06:52 regfile-link.txt -> /mnt/basic/regfile.txt
    708 -rw-r--r--    1 root     root            16 Feb 19 06:50 regfile.txt
    709 -rw-r--r--    1 root     root            18 Feb 19 06:50 regfile2.txt
# cat /mnt/basic/regfile-link.txt
Creating a file
```

The output of `dmesg` is:

```
[ 2611.055337] basic_fs: loading out-of-tree module taints kernel.
[ 2611.061991] basic_fs: basic_fs_init: Init the basic filesystem module
[ 2659.448264] basic_fs: basic_fs_mount: mounting file system
[ 2659.449103] basic_fs: basic_fs_fill_super: Filling super block
[ 2659.449480] basic_fs: basic_fs_create_inode: set inode and file operations for a dir
[ 2659.449727] basic_fs: basic_fs_create_inode: inode->i_ino = 641, inode->i_nlink = 2
[ 2659.449911] basic_fs: basic_fs_fill_super: root_inode->i_nlink = 2
[ 2675.288911] basic_fs: basic_fs_create_inode: set inode and file operations for regular file
[ 2675.289199] basic_fs: basic_fs_create_inode: inode->i_ino = 642, inode->i_nlink = 1
[ 2682.611551] basic_fs: basic_fs_create_inode: set inode and file operations for regular file
[ 2682.611962] basic_fs: basic_fs_create_inode: inode->i_ino = 643, inode->i_nlink = 1
[ 2696.553278] basic_fs: basic_fs_create_inode: set inode and file operations for a dir
[ 2696.553773] basic_fs: basic_fs_create_inode: inode->i_ino = 644, inode->i_nlink = 2
[ 2726.974286] basic_fs: basic_fs_symlink: Creating symlink /mnt/basic/regfile.txt
[ 2726.974812] basic_fs: basic_fs_create_inode: set inode and file operations for link file
```
