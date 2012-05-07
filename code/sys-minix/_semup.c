#include <lib.h>
#include <unistd.h>

#define SEMUP 56

PUBLIC int semup(int sem){
  message m;
  m.m1_i1=sem;
  return(_syscall(PM_PROC_NR,SEMUP,&m));
}
