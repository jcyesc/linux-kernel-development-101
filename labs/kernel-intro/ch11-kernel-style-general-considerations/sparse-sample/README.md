
# Find errors with `sparse`

In this exercise, there is the module `lab_sparse_erros.c` that has some
semantic errors. Use sparse to find the error. See the `Makefile` to find out
how use `sparse`.

 ## Install sparse:
 
 ```shell
$ git clone https://git.kernel.org/pub/scm/devel/sparse/sparse.git
$ cd sparse
$ make
$ sudo make PREFIX=/usr/local install
```
 
Note: If you find this problem:
 
> 'https://git.kernel.org/pub/scm/devel/sparse/sparse.git/': server certificate verification failed. CAfile: /etc/ssl/certs/ca-certificates.crt CRLfile: none

You can solve it with:

```shell
$ sudo apt update
$ sudo apt upgrade
$ sudo apt-get install --reinstall ca-certificates
$ sudo update-ca-certificates
```
