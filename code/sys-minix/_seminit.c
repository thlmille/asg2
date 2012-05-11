#include <lib.h>
#include <unistd.h>

#define SEMINIT 44


PUBLIC int seminit(int sem, int value){
  message m;
  m.m1_i1=sem;
  m.m1_i2=value;
  int err = (_syscall(PM_PROC_NR,SEMINIT,&m));
  return err;
}
