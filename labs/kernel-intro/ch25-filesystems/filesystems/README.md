
# File System and Virtual File system

A file sysstem is framework designed to organize files, directories, links,
etc in a hard disk, sd card, or memory.

Most of the file system need to implement caching, to make the access to files
and directories faster. Because these and other operations are common to all
file systems, Linux implements the Virtual File System (VFS).


## Virtual File System (VFS)

The Virtual File System is a framework that provides common functionality such:

- cache of data structures
- separation of the block layer from the filesystem.
- queue mechanism to send the request to the block layer.


```
    _____________________________________________________________
    |                                                           |
    |                       User space                          |
    |  mkdir(), rmdir() open(), lseek(), read(), write(), etc.  |
    |                                                           |
    |___________________________________________________________|
            |                                       |
            |                                       |
            |                                       |
    _____________________________________________________________
    |                                                           |    ______________
    |                 Virtual File System (VFS)                 |____|            |
    |              (caching, queueing, request, etc)            |    | Page Cache |
    |    building blocks: super_block, dentry, inode, file      |    |____________|
    |___________________________________________________________|
           |               |              |               |
           |               |              |               |
           |               |              |               |
      ___________     ___________    ___________      ___________
      |         |     |         |    |         _|     |         |
      |  ext4   |     |  f2fs   |    | squashfs |     |   nfs   |
      | module  |     | module  |    | module   |     |  module |
      |_________|     |_________|    |__________|     |_________|
           |               |               |
           |               |               |
           |               |               |
    ______________   ______________  ______________
    |            |   |            |  |            |
    |    emmc    |   |     sd     |  |    ram     |
    | blk driver |   | blk driver |  | blk driver |
    |____________|   |____________|  |____________|
           |                |
           |                |
     _____________     ___________
     |           |     |         |
     | emmc disk |     | sd card |
     |___________|     |_________|
```
 

The main data structures defined for the VFS are:

- `struct super_block`:
- `struct dentry`:
- `struct inode`:
- `struct file`:
- `struct address_space`:

TODO: Draw diagram with file descriptors and pointers to structures

# Block Layer

In the block layer, the data structures ... have different representation in the
block device.

TODO: Draw diagram of the content of the block device.
























