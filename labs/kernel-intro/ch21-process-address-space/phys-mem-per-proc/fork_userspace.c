/**
 * Fork a child thread.
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PAGE_SIZE 4096

/*
 * The buffer is placed in a PAGE_SIZE offset so we can see clearly in the
 * objdump where it is placed in virtual memory.
 */
char __attribute__ ((aligned (PAGE_SIZE))) buffer[PAGE_SIZE * 3];

int main(void) {
  int pid;
  int status;
  int val;
  pid = fork();
  if (pid == 0) {
    printf("child: Child, PID %d\n", getpid());
    val = getchar();

    while (val != 'E') {
      if (val == 'M') {
        buffer[0] = 'T';
      }

      val = getchar();
    }

    exit(0);
  } else if (pid > 0) {
    printf("parent: Parent PID %d, Child PID %d\n", getpid(), pid);
    wait(&status);
    printf("parent: Child terminated, status %d\n", WEXITSTATUS(status));
  } else {
    perror("Error while forking");
  }

  return 0;
}
