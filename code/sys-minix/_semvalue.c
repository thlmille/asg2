#include <lib.h>
#include <unistd.h>

#define SEMVALUE 45

PUBLIC int semvalue(int sem){
  message m;
  m.m1_i1 = sem;
  return(_syscall(PM_PROC_NR,SEMVALUE,&m));
}
