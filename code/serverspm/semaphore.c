#include <sys/queue.h>
#include <errno.h>
#include "semaphore.h"

int seminit(int sem,int value){
  if (sems_in_use > 99){
    return EAGAIN;
  }
  if (sem > 1000 || sem < 0){
    return EINVAL;
  }
  if (check_sem(sem) != 0){
    return EEXIST;
  } else {
    if (value == 0){ //value needs to be non-zero pos.
      return -1;
    } else {
      sem_list[sem]->block = value; //how many processes are allowed before blocking
      sem_list[sem]->current = 0; //tells how many processes are currently waiting
      sem_list[sem]->id = sem; //not needed
      LIST_HEAD(sem_l,sem) sem_head;
      LIST_INIT(&sem_head);
    }
  }
}

int semvalue(int sem){
  
}

int check_sem(int sem){
  if(sem_list[sem] != 0){ //zero means it is not in use
    return 1;
  }
  return 0;
}

int semup(int sem){}
int semdown(int sem){}
int semfree(int sem){}
