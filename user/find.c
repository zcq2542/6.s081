#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


char*
fmtname(char *path) // return the chars at the right of last '/'
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  //memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  buf[strlen(p)] = 0;
  return buf;
}

char* d = ".";
char* dd = "..";

void
find(char *path, char* name)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }
  char* cur;
  switch(st.type){
  case T_FILE:
    cur = fmtname(path);
    // printf("%s len:%d,   %s len:%d\n", name, strlen(name), cur, strlen(cur));  
    if(strcmp(name, cur) == 0){
      printf("%s\n", path);
    }

    //printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path); // buf: "."
    p = buf+strlen(buf);
    *p++ = '/'; // buf: "./"
    while(read(fd, &de, sizeof(de)) == sizeof(de)){ // read entity in the dir(path).
      if(de.inum == 0)
        continue;
      if(strcmp(de.name, d) == 0 || strcmp(de.name, dd) == 0) // de.name="." || ".."
        continue;
      memmove(p, de.name, DIRSIZ); //buf: "./{de.name}"
      p[DIRSIZ] = 0; // buf: "./{de.name}0"
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      //printf("%s\n", buf);

      find(buf, name);
      //printf("%s\n", buf);
      //printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 3){
    find(".", argv[1]);
    exit(0);
  }
  for(i=2; i<argc; i++)
    find(argv[1], argv[i]);
  exit(0);
}

