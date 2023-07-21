
# Loopback device


A `loopback device` is a file system that is created on top of another
filesystem. This `loopback device` can be mounted, and once that it is
mounted, can be modified (create/delete/update files and directories).

To support loopback devices, the Linux kernel has to be configured with the
`CONFIG_BLK_DEV_LOOP`.

```shell
 $ cat .config | grep CONFIG_BLK_DEV_LOOP
CONFIG_BLK_DEV_LOOP=y
```

## How to create loop device


```shell
 $ dd if=/dev/zero of=newfs.ext4 bs=4096 count=1024
1024+0 records in
1024+0 records out
4194304 bytes (4.2 MB, 4.0 MiB) copied, 0.00445523 s, 941 MB/s

 $ mkfs.ext4 newfs.ext4
mke2fs 1.45.5 (07-Jan-2020)

Filesystem too small for a journal
Discarding device blocks: done
Creating filesystem with 1024 4k blocks and 1024 inodes

Allocating group tables: done
Writing inode tables: done
Writing superblocks and filesystem accounting information: done
```

If we want the filesystem to support encryption, we can use `tun2fs` command:

```shll
$ man tune2fs

   -O [^]feature[,...]
              Set  or  clear the indicated filesystem features (options) in the filesystem.

              The following filesystem features can be set or cleared using tune2fs:
       64bit  Enable the file system to be larger than 2^32 blocks.
                    ....
                    ....

       encrypt
               Enable  support  for file system level encryption.  Tune2fs currently
               only supports setting this filesystem feature.



 $ tune2fs -O encrypt newfs.ext4
tune2fs 1.45.5 (07-Jan-2020)
```

To `mount` the new filesystem `newfs.ext4`, follow instructions below:

```shell
 $ mkdir temp_mount
 $ sudo mount -o loop newfs.ext4 ./temp_mount/

 $ Verify that it was mounted
 $ mount | grep temp_mount
/home/user/newfs.ext4 on /home/user/temp_mount type ext4 (rw,relatime)
```

Create files or directories in the new file system:

```shell
 $ ls
newfs.ext4  temp_mount
 $ cd temp_mount/
 $ ls
lost+found
 $ sudo mkdir docs
 $ sudo vim docs/msg.txt

 # Umount the filesystem
 $ cd ..
 $ sudo umount temp_mount

 # Mount again and verify that the previously created files are still there.
 $ sudo mount -o loop newfs.ext4 temp_mount/
 $ ls temp_mount/docs/msg.txt
temp_mount/docs/msg.txt
 $ cat temp_mount/docs/msg.txt
Hello from loopback device
```

