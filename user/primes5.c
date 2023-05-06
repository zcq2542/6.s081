#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void nextLevel(int *);

int
main(int argc, char *argv[])
{
  int hasChild = 0;
  int fd[2];
  int n = 2;
  if (pipe(fd) < 0) {
    printf("pipe error\n");
    exit(1);
  }

  printf("prime: %d\n", n);
  for (int i = 3; i < 35; ++i) {
    if (i % n != 0) {
      if (hasChild == 0) {
        nextLevel(fd);
      }
      printf("%d\n", i);
      write(fd[1], &i, 4);
    }
  }

  close(fd[1]);
  wait(0);
  close(fd[0]);
  exit(0);
}

void nextLevel(int *fromPipe)
{
  int hasChild = 0;
  int n;

  int pid = fork();
  if (pid == 0) { // child process.
    close(fromPipe[1]);
    if (read(fromPipe[0], &n, 4) == 0) { // read from pipe
      close(fromPipe[0]);
      exit(0); // exit if pipe is closed
    }
    printf("pid: %d, prime: %d\n", getpid(), n);
    int pipeToChild[2]; // to send int to child;
    pipe(pipeToChild);
    close(pipeToChild[0]);
    int i;
    while (read(fromPipe[0], &i, 4)) {
      if (i % n != 0) {
        if (hasChild) {
          write(pipeToChild[1], &i, 4);
        } else {
          nextLevel(pipeToChild);
        }
      }
    }
    close(fromPipe[0]);
    close(pipeToChild[1]);
    exit(0);
  } else {
    hasChild = 1;
    close(fromPipe[0]);
  }
}

