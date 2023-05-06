#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
//#include <stddef.h>

void nextLevel(int read_fd);

int
main(int argc, char *argv[])
{
  int fd[2];
  if (pipe(fd) < 0) {
    printf("pipe error\n");
    exit(1);
  }

  // Write the numbers 2 through 35 to the write-end of the pipe
  for (int i = 2; i <= 35; i++) {
    write(fd[1], &i, sizeof(int));
  }

  //int i = -1;
  //printf("i = %d\n", i);
  //write(fd[1], &i, sizeof(int));
  close(fd[1]);
  // Start the pipeline
  nextLevel(fd[0]); // 
  // close(fd[0]);
  close(fd[1]);
  // Wait for all child processes to terminate
  // while (wait(0) > 0);
  wait((int*)(0));
  exit(0);
}

void
nextLevel(int read_fd)
{
  int prime;
  int child_fd[2];
  int hasChild = 0;
  int n;
  // Read the next prime number from the read-end of the pipe
  if (read(read_fd, &prime, sizeof(int)) == 0) {
    // The read-end of the pipe has been closed, so we're at the end of the pipeline
    printf("exit\n");
    exit(0);
  }
  if (prime < 0) {
    printf("%d: receive %d\n", getpid(), prime);
    exit(0);
  }

  printf("prime: %d\n", prime);

  int child_pid = 0;
  // Loop through the remaining numbers in the pipe
  while (read(read_fd, &n, sizeof(int))) {
    //if (n == -1) {
      // write(child_fd[1], &n, sizeof(int));
      // printf("pid: %d receive -1\n", getpid()); 
      //break;
    //}
    if (n % prime != 0) {
      if (hasChild == 0) {
        // Create a new pipe and fork a child process
        pipe(child_fd);
        child_pid = fork();
        if (child_pid == 0) {
          // Child process
          close(child_fd[1]);
          nextLevel(child_fd[0]);
          //close(child_fd[0]);
        } else {
          // Parent process
          hasChild = 1;
          close(child_fd[0]);
        }
      }
      write(child_fd[1], &n, sizeof(int));
    }
  }
  // printf("end");
  // Close the write-end of the child pipe and wait for child processes to terminate
  close(child_fd[1]);
  // if (child_pid > 0) {
    // while (wait(0) > 0);
    //wait(0);
  //}
  wait((int*)(0));
  // printf("pid: %d exit\n", getpid());
  exit(0);
}

