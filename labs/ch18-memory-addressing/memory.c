#include <stdio.h>

/*
gcc -o memory memory.c
objdump -D memory > dissassembly.txt
*/

int global1 = 20;
// Ox14 = (16 ^ 1 * 1)+ (16 ^ 0 + 4) = 20
int global2 = 31;

int main()
{
  int local = 24;

  printf("global %d, %p\n", global1, &global1);
  printf("global %d, %p\n", global2, &global2);
  printf("local %d, %p\n", local, &local);
}

