
# Completion Functions

To compile and upload the modules to the raspberry pi.

```shell
$ make
$ scp *.ko pi@192.168.2.2:~
```

Load the modules

```shell
$ ssh pi@192.168.2.2
pi@raspberrypi:~/ $ sudo insmod completion_functions_module.ko
pi@raspberrypi:~/ $ sudo insmod producer_module.ko
pi@raspberrypi:~/ $ tail -f /var/log/messages
May  9 13:17:36 raspberrypi kernel: [   41.554484] completion_functions_module: loading out-of-tree module taints kernel.
May  9 13:17:36 raspberrypi kernel: [   41.555043] cfm: Loading the module completion_functions_module
May  9 13:18:22 raspberrypi kernel: [   41.555276] cfm-kthread: Waiting for completion function
May  9 13:18:22 raspberrypi kernel: [   86.703580] pm: Loading the module producer_module
May  9 13:18:22 raspberrypi kernel: [   86.703606] pm: Producer building the message!
May  9 13:18:22 raspberrypi kernel: [   86.703640] pm: Task completed
May  9 13:18:22 raspberrypi kernel: [   86.703671] cfm-kthread: Completion function is DONE.
May  9 13:18:22 raspberrypi kernel: [   86.703689] cfm-kthread: The shared message is: Thundercats are alive!!
```
