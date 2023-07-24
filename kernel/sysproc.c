#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
//  a system call that reports which pages have been accessed.
// return the address of buffer.
{
  // lab pgtbl: your code here.
  uint64 baseaddr;
  if(argaddr(0, &baseaddr) < 0)
    return -1;

  int pagenum;
  if(argint(1, &pagenum) < 0)
    return -1;

  uint64 ansaddr;
  if(argaddr(2, &ansaddr) < 0)
    return -1;
  //int[1000000] buffer;
  char mask; // 8bits mask
  int num = 0; // scaned page numbers update every 8 pages.
  pagetable_t pagetable = myproc()->pagetable;
  vmprint(pagetable);
  for(int i = 0; i < pagenum; ++i) {
    pte_t *pte = walk(pagetable, baseaddr + i * PGSIZE, 0);// get the leave node pte. 
    if(*pte & PTE_A) { // this page is accessed
      mask |= 1 << num; // the num_th position of mask marked as 1;
      *pte &= ~PTE_A; // pte mark as not accessed.
    }

    if(++num == 8) { // record 8 pages. (num add 1)
      if(copyout(pagetable, ansaddr, (char*) &mask, 1) < 0) // copy this mask to destionation addr.
        return -1;
      ++ansaddr; // point to next byte.
      num = 0;
      mask = 0;
    }
  }

  if(num) { // out of for loop. the last mask uncopyed.
    if(copyout(pagetable, ansaddr, (char*) &mask, 1) < 0) // copy this mask to destionation addr.
        return -1;
  }
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

