#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * Fork a child thread.
 */

char buffer[10];

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
