#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAX_ARG_LEN 512

int readline(char *buf){
  char *p = buf; // let p point the same as buf
;
  int n;
  while((n = read(0, p, 1)) != 0){// read 1 char to p every time.
    if(*p == '\n'){ // read to the end of line
      *p = 0; // as end of arg
      return 0;
    }
    ++p; // pointer p move to next position.
    if(p >= buf + MAX_ARG_LEN){ // 
      fprintf(2, "line too long\n");
      exit(1);
    }
  }
  *p = 0;
  //printf("end\n");
  return 1; // end of arg didn't meet '\n'
}




int main(int argc, char *argv[]){
  char line[MAX_ARG_LEN];
  int read_end;
  int pid;
  char *xargs[MAXARG];

  for(int i = 1; i < argc; ++i){
    xargs[i-1] = argv[i];
  } // copy the argv remove "xargs"
  
  while(1){ // read line from stdin(pipe)
    read_end = readline(line);
    // printf("read_end: %d\n", read_end);
    if(line[0] == 0 && read_end == 1) break; // read next line
    pid = fork();
    if(pid == 0){ // child process
      xargs[argc-1] = line; // append line as last arg
      exec(argv[1], xargs);
      fprintf(2, "exec: %s failed\n", argv[1]);
      exit(1);

    }
    else{ // parent process
      wait(0);
      if(read_end == 1) {
        // printf("break\n");
        break; // last line, end loop.
      }
    }
  }
  exit(0);
}
