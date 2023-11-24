
## How to generate call graphs

To generate call graphs, we need 2 tools:

- graphviz
- egypt

## Install graphviz

```shell
 $ sudo apt-get install graphviz
```

## Install egypth

```shell
 $ wget http://www.gson.org/egypt/download/egypt-1.10.tar.gz
 $ tar -xzf egypt-1.10.tar.gz
 $ cd egypt-1.10
 $ perl Makefile.PL
 $ make
 $ sudo make install
```

## Generate the graphs

Add the compiler flag `-fdump-rtl-expand` to the `Makefile` of the directory
you are interested:

```shell
 $ git diff
diff --git a/fs/Makefile b/fs/Makefile
index 8d4736fcc766..04e63c55c97f 100644
--- a/fs/Makefile
+++ b/fs/Makefile
@@ -6,6 +6,7 @@
 # Rewritten to use lists instead of if-statements.
 #

+ccflags-y += -fdump-rtl-expand

 obj-y :=       open.o read_write.o file_table.o super.o \
                char_dev.o stat.o exec.o pipe.o namei.o fcntl.o \
```

Generate the graph using `egypt` and `graphviz`:

```shell
 $ egypt ~/linux-stable/fs/*.expand | dot -Grankdir=LR -Tsvg -o callgraph.svg
```

