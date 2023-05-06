#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(){
  int fd[2];
  pipe(fd);
  char buffer[5];
  
  if(fork() == 0){
    close(fd[1]);
    int r = read(fd[0], buffer, 5);
    while(r){
      printf("r: %d\n", r);
      printf("%s\n", buffer);
      r = read(fd[0], buffer, 5);
    }
    printf("child end\n");
  }
  else{
    close(fd[0]);
    int i = 5;
    while(i--){
      write(fd[1], "hello", 5);

    }
    write(fd[1], "hello", 5);
    close(fd[1]);
  }
  exit(0);
}
