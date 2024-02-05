
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
```

The output of `dmesg` is:

```
[   35.985137] basic_fs: loading out-of-tree module taints kernel.
[   35.994636] basic_fs: basic_fs_init: Init the basic filesystem module
[   71.052061] basic_fs: basic_fs_mount: mounting file system
[   71.052536] basic_fs: basic_fs_fill_super: Filling super block
[   71.052923] basic_fs: basic_fs_create_inode: set inode and file operations for a dir
[   71.055252] basic_fs: basic_fs_create_inode: inode->i_ino = 703, inode->i_nlink = 2
[   71.055889] basic_fs: basic_fs_fill_super: root_inode->i_nlink = 2
[   80.395678] basic_fs: basic_fs_create: Creating node
[   80.396257] basic_fs: basic_fs_create_inode: set inode and file operations for regular file
[   80.396644] basic_fs: basic_fs_create_inode: inode->i_ino = 704, inode->i_nlink = 1
[   80.398876] basic_fs: basic_fs_mknod: Making node
[   88.210933] basic_fs: basic_fs_create: Creating node
[   88.211326] basic_fs: basic_fs_create_inode: set inode and file operations for regular file
[   88.211585] basic_fs: basic_fs_create_inode: inode->i_ino = 705, inode->i_nlink = 1
[   88.212110] basic_fs: basic_fs_mknod: Making node
[  108.684869] basic_fs: basic_fs_mkdir: Making dir
[  108.685202] basic_fs: basic_fs_create_inode: set inode and file operations for a dir
[  108.686794] basic_fs: basic_fs_create_inode: inode->i_ino = 706, inode->i_nlink = 2
```
