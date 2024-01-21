
# Read/Write using a `struct bio`


The `bio_block_driver.c` sends request to the block layer by creating
`struct bio` request and adding the kernel mapped page to the request.

The `bio_block_driver.c` depends on the
`ch25-filesystems/ram-block-driver/ram_block_driver.c` driver.

To run the example, follow the instructions.

 1. Install the `block_driver.ko` and write a message to the ramdisk:

 ```
  $ insmod ram_block_driver.ko
  $ mknod /dev/ramdiskblockdev b 240 0
  $ echo "Linux is a powerful and versatile OS" > /dev/ramdiskblockdev
  $ dd if=/dev/ramdiskblockdev bs=1 count=50
  Linux is a powerful and versatile OS
  �V50+0 records in
  50+0 records out
 ```

 2. Install the `bio_block_driver.ko` that will send a write and read bio request.

 ```
  $ insmod bio_block_driver.ko
  $ dd if=/dev/ramdiskblockdev bs=1 count=50
  Writing to the File System :)N50+0 records in
  50+0 records out
 ```
