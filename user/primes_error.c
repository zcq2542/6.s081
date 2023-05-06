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
    // printf("pid: %d, i: %d\n", getpid(), i);
    if (i % n != 0){
      if (hasChild == 0) {
          nextLevel(fd);
      }
      printf("%d\n", i);
      write(fd[1], &i, 4);
    }
  }
  //int i = -1;
  //write(fd[1], &i, 4);
  
  close(fd[1]);
  
  //while (wait(0) > 0) {
    //    int val;
     //   if (read(fd[0], &val, 4) > 0) {
     //       printf("pid: %d, prime: %d\n", getpid(), val);
      //  }
   // }
  close(fd[0]);
  exit(0);
}

void nextLevel(int *fromPipe){
  int hasChild = 0;
  int n;
  
  int pid = fork();
  if (pid == 0) { // child process.
    //printf("pid: %d, fromPipe[1]: %d\n", getpid(), fromPipe[1]);
    //printf("pid: %d, fromPipe[0]: %d\n", getpid(), fromPipe[0]);
    close(fromPipe[1]);
    read(fromPipe[0], &n, 4); // read from pipe
    if(n < 0) exit(0);
    printf("pid: %d, prime: %d\n", getpid(), n);
    int pipeToChild[2]; // to send int to child;
    pipe(pipeToChild);
    close(pipeToChild[0]);
    int i;
    while(read(fromPipe[0], &i, 4)){ // 
      if (i % n != 0) {
        if (hasChild)  write(pipeToChild[1], &i, 4);
        else nextLevel(pipeToChild);
      }
    }
    close(fromPipe[0]);
    close(pipeToChild[1]);
    exit(0);
  }
  else {
    hasChild = 1;
    close(fromPipe[0]);
  }
}
