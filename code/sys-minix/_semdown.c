#include <lib.h>
#include <unistd.h>

#define SEMDOWN 57

PUBLIC int semdown(int sem){
  message m;
  m.m1_i1=sem;
  return(_syscall(PM_PROC_NR,SEMDOWN,&m));
}
