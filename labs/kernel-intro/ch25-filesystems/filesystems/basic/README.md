
# Basic File System

The `basic_fs.ko` module implements a file system that does not need a block
device to be mounted. To register the file system, first mount the module:

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
    668 basic
$ ls -i /mnt/basic/
    673 private           671 regfile.txt       672 regfile2.txt
$ ln -s /mnt/basic/regfile.txt /mnt/basic/regfile-link.txt
$ ls -li /mnt/basic/
total 8
    673 drwxr-xr-x    2 root     root             0 May 17 22:18 private
    674 lrwxrwxrwx    1 root     root            22 May 17 22:19 regfile-link.txt -> /mnt/basic/regfile.txt
    671 -rw-r--r--    1 root     root            16 May 17 22:17 regfile.txt
    672 -rw-r--r--    1 root     root            18 May 17 22:17 regfile2.txt
$ cat /mnt/basic/regfile-link.txt
Creating a file
$ umount /mnt/basic
```

The output of `dmesg` is:

```
[10249.885430] basic_fs: basic_fs_init: Init the basic filesystem module
[10321.747394] basic_fs: basic_fs_mount: mounting file system
[10321.749275] basic_fs: basic_fs_fill_super: Filling super block
[10321.749861] basic_fs: basic_fs_create_inode: set inode and file operations for a dir
[10321.750271] basic_fs: basic_fs_create_inode: inode->i_ino = 668, inode->i_nlink = 2
[10321.750750] basic_fs: basic_fs_fill_super: root_inode->i_nlink = 2
[10386.758702] basic_fs: basic_fs_create_inode: set inode and file operations for regular file
[10386.759089] basic_fs: basic_fs_create_inode: inode->i_ino = 671, inode->i_nlink = 1
[10394.864869] basic_fs: basic_fs_create_inode: set inode and file operations for regular file
[10394.865175] basic_fs: basic_fs_create_inode: inode->i_ino = 672, inode->i_nlink = 1
[10420.298609] basic_fs: basic_fs_create_inode: set inode and file operations for a dir
[10420.299471] basic_fs: basic_fs_create_inode: inode->i_ino = 673, inode->i_nlink = 2
[10520.438750] basic_fs: basic_fs_symlink: Creating symlink /mnt/basic/regfile.txt
[10520.439836] basic_fs: basic_fs_create_inode: set inode and file operations for link file
[10520.440538] basic_fs: basic_fs_create_inode: inode->i_ino = 674, inode->i_nlink = 1
[12055.640405] basic_fs: basic_fs_exit: Exiting the module
```
