#include <lib.h>
#include <unistd.h>

#define SEMFREE 58

PUBLIC int semfree(int sem){
  message m;
  m.m1_i1=sem;
  return(_syscall(PM_PROC_NR,SEMFREE,&m));
}
