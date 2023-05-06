#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int pid;
  int fd[2];
  int fd2[2];
  char buffer[5];
  pipe(fd); // create the pipe
  pipe(fd2);
  
  pid = fork();
  if(pid == 0){ // child process
    // close(0); // close file descriptor 0 (can't read from system in).
    // dup(fd[0]); // duplicate file descriptor fd[0] to file descriptor 0.
    // close(1);
    // dup(fd[1]); //
//    close(fd[1]); //close write end of pipe. otherwise read will be  blocked.
    int n = read(fd[0], buffer, 1);
    if(n == -1){
     // perror("read");
      printf("%d: read error", getpid());
      exit(1);
    }
    // printf("%s\n", buffer);
    printf("%d: received ping\n", getpid());
    write(fd2[1], "b", 1);
    // printf("%d: written\n", getpid());
    exit(0);
  }
  
  write(fd[1], "a", 1);
  //close(fd[1]);
  // wait();
  // printf("%d: written\n", getpid());
  int n = read(fd2[0], buffer, 1);
  // printf("%s\n", buffer);
  if(n == -1){
    // perror("read");
    printf("%d: read error", getpid());
    exit(1);
  }
  
  printf("%d: received pong\n", getpid());
  exit(0);
}
