
# How to use krefs


krefs is a kernel library that counts references.

This example has the following goals:

- Create a `thunder_cat` object in the krefs_module.c module.
- Shared the reference with the modules krefs_client_module{1,2}.c
- Define the `thunder_cat` library in an `include` directory.

Links:

- https://www.kernel.org/doc/html/latest/core-api/kref.html
- https://elixir.bootlin.com/linux/latest/source/include/linux/kref.h
- https://www.kernel.org/doc/html/latest/kbuild/modules.html#several-subdirectories

To test the modules use 2 consoles, one to check the logs and another to load
the modules:

```shell
pi@raspberrypi:~/Development $ sudo insmod thunder_cats_module.ko 
pi@raspberrypi:~/Development $ sudo insmod thunder_cats_client1_module.ko 
pi@raspberrypi:~/Development $ sudo insmod thunder_cats_client2_module.ko 
pi@raspberrypi:~/Development $ sudo rmmod thunder_cats_module 
rmmod: ERROR: Module thunder_cats_module is in use by: thunder_cats_client1_module thunder_cats_client2_module
pi@raspberrypi:~/Development $ sudo rmmod thunder_cats_client2_module 
pi@raspberrypi:~/Development $ sudo rmmod thunder_cats_client1_module 
pi@raspberrypi:~/Development $ sudo rmmod thunder_cats_module 
```

```shell
pi@raspberrypi:~ $ tail -f /var/log/messages
May 10 22:50:14 raspberrypi kernel: [  299.953310] thunder_cats_client1_module: loading out-of-tree module taints kernel.
May 10 22:50:14 raspberrypi kernel: [  299.953605] thunder_cats_client1_module: Unknown symbol panthro (err -2)
May 10 22:50:14 raspberrypi kernel: [  299.953658] thunder_cats_client1_module: Unknown symbol release_thunder_cat (err -2)
May 10 22:50:51 raspberrypi kernel: [  336.310548] thunder_cats_module: Loading the module thunder_cats_module
May 10 22:50:51 raspberrypi kernel: [  336.310567] Thunder Cat [Panthro, 30] has [1] references(s)!
May 10 22:50:51 raspberrypi kernel: [  336.310778] thunder_cat_handler() has the thunder cat Panthro
May 10 22:50:51 raspberrypi kernel: [  336.310787] Thunder Cat [Panthro, 30] has [2] references(s)!
May 10 22:51:06 raspberrypi kernel: [  351.464992] thunder_cats_client1_module: Loading the module thunder_cats_client1_module
May 10 22:51:06 raspberrypi kernel: [  351.465010] Thunder Cat [Panthro, 30] has [2] references(s)!
May 10 22:51:46 raspberrypi kernel: [  391.841038] thunder_cats_client2_module: Loading the module thunder_cats_client2_module
May 10 22:51:46 raspberrypi kernel: [  391.841056] Thunder Cat [Panthro, 30] has [3] references(s)!
May 10 22:52:17 raspberrypi kernel: [  422.744907] thunder_cats_client2_module: Exiting the module thunder_cats_client2_module
May 10 22:52:29 raspberrypi kernel: [  434.822086] thunder_cats_client1_module: Exiting the module thunder_cats_client1_module
May 10 22:52:35 raspberrypi kernel: [  440.792878] thunder_cats_module: Exiting the module thunder_cats_module
May 10 22:52:35 raspberrypi kernel: [  440.792904] Thunder Cat [Panthro, 30] has [1] references(s)!
May 10 22:52:35 raspberrypi kernel: [  440.792919] thunder_cats_module: release_thunder_cat() is releasing thunder cat Panthro
May 10 22:52:35 raspberrypi kernel: [  440.792932] Thunder Cat [Panthro, 30] has [0] references(s)!
```