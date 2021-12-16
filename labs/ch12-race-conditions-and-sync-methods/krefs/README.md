
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