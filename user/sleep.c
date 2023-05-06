#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    fprintf(2, "Usage: sleep [sleep sec]...\n");
    exit(1);
  }
  for(i = 1; i < argc; i++){
    // printf("%d", atoi(argv[i]));
    sleep(atoi(argv[i]) * 10);
  }
  exit(0);
}
