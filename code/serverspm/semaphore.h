#include <lib.h>
#include "namespace.h"
#include <sys/queue.h>

typdef struct sem{
  int block;
  int current;
  int id;
  LIST_ENTRY (sem) next;
}sem;

sem* sem_list;


PUBLIC int seminit(int sem,int value){
  message m;
  m.m1_i1 = sem;
}
int semvalue(int sem);
int semup(int sem);
int semdown(int sem);
int semfree(int sem);

int sems_in_use;
int check_sem(int sem);
