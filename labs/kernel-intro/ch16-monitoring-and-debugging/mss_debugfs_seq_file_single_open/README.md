
# MSS debugfs and seq_file example


To compile and deploy the module, use:

```shell
$ make
$ scp mss_debugfs_seq_file_single_open.ko pi@192.168.2.2:~

pi$ sudo insmod mss_debugfs_seq_file_single_open.ko
```

To read the debugfs file, you can use:

```shell
pit$ sudo cat /sys/kernel/debug/mssdbg/mc2/lpddr4-3
id         0x0000000b
status     0x0000000c
ecc        0x0000000d
cnt        0x0000000e
speed      0x0000000f
```

To write to the debugfs file using `mss_user.c`, run:

```shell
# Compile and upload the binary.
$ arm-linux-gnueabi-gcc mss_user.c -o mss_user.bin
$ scp mss_user.bin pi@192.168.2.2:~

pi$ sudo ./mss_user.bin
pi$ dmesg
```

To write to the debugfs file using the `write.sh` script, run:

```shell
$ scp write.sh pi@192.168.2.2:~

pi$ chmod +x ./write.sh
pi$ sudo ./write.sh
pi$ dmesg

```
