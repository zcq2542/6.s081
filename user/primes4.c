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
  if(pipe(fd) < 0) {
    printf("pipe error\n");
    exit(1);
  }

  printf("prime: %d\n", n);
  for (int i = 3; i < 35; ++i){
    if (i % n != 0){
      if (hasChild == 0) {
        nextLevel(fd);
        hasChild = 1;
      }
      printf("%d\n", i);
      write(fd[1], &i, sizeof(int));
    }
  }

  // Write sentinel value to indicate end of input.
  int sentinel = -1;
  write(fd[1], &sentinel, sizeof(int));

  close(fd[1]);

  wait(0);  // Wait for child process to exit.

  close(fd[0]);
  exit(0);
}

void nextLevel(int *fromPipe){
  int n;

  int pid = fork();
  if (pid == 0) { // child process.
    close(fromPipe[1]);
    read(fromPipe[0], &n, sizeof(int)); // read from pipe
    if(n < 0) exit(0);
    printf("pid: %d, prime: %d\n", getpid(), n);
    int pipeToChild[2]; // to send int to child;
    pipe(pipeToChild);
    close(pipeToChild[0]);
    int i;
    while(read(fromPipe[0], &i, sizeof(int))){ // 
      if (i % n != 0) {
        write(pipeToChild[1], &i, sizeof(int));
      }
    }

    // Write sentinel value to indicate end of input.
    int sentinel = -1;
    write(pipeToChild[1], &sentinel, sizeof(int));

    close(fromPipe[0]);
    close(pipeToChild[1]);
    exit(0);
  }
  else {
    close(fromPipe[0]);
  }
}
