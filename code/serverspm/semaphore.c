#include "pm.h"
#include <sys/queue.h>
#include <errno.h>
#include <lib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "param.h"
#include "mproc.h"

typedef struct sem{
  int id;
  int val;
  int proc[1000];
}sem;

PRIVATE sem* sem_array[101];
//memset(&sem_array,NULL,100*sizeof(sem));

PUBLIC int do_seminit(){
  int sema=m_in.m1_i1; //first value to pass in
  int value = m_in.m1_i2; //second value to pass in
  printf("sem : %d, value: %d\n",sema,value);
  if (sema > 1000 || sema < 0){
    return EINVAL;
  }
  if (sema == 0){
    int i;
    for(i=1;i<101;i++){
      if (sem_array[i] == NULL){
	sema = i;
      }
    }
    if (sema == 0){
      return EAGAIN;
    }
  }
  if (sem_array[sema] == NULL){
    sem *a = malloc (sizeof (struct sem*));
    a->id = sema;
    a->val = value;
    memset(&a->proc,0,100*sizeof(int));
    memcpy(sem_array[sema],&a,sizeof(sem*));
  } else {
    printf("value already exists\n");
    return EEXIST;
  }

  return 0;
}

PUBLIC int do_semvalue(){
  int sem=m_in.m1_i1;
  printf("sem: %d\n",sem);
  if (sem_array[sem] == NULL){
    return 0x8000000;
  } else {
    printf("returning : %d\n" ,sem_array[sem]->val);
    return sem_array[sem]->val;
  }
}

PUBLIC int do_semup(){
  int sem=m_in.m1_i1;
  if (sem_array[sem] != NULL){
    sem_array[sem]->val += 1;
    if (sem_array[sem]->val > 1000){
      return EOVERFLOW;
    }
  } else {
    return 1;
  }
  if (sem_array[sem]->val <= 0){
    int process = sem_array[sem]->proc[0];
    mp[process].mp_flags |= REPLY;
    memcpy(&sem_array,&sem_array+1,99*sizeof(int));

    //do_(); //this is where we wake up waiting proc
  }
  //printf("sem: %d\n",sem);
  return 0;
}

PUBLIC int do_semdown(){
  int sem=m_in.m1_i1;
  if (sem_array[sem] != NULL){
    sem_array[sem]->val -= 1;
    if (sem_array[sem]->val < 1000){
      return EOVERFLOW;
    }
  } else {
    return 1;
  }
  if (sem_array[sem]->val < 0){
    mp->mp_flags |= PAUSE;
    int i = 0;
    while(i<100){
      if (sem_array[sem]->proc[i] == 0){
	sem_array[sem]->proc[i] = mp->mp_pid;
	i = 101;
      }
    }
  }
  return 0;
}

PUBLIC int do_semfree(){
  int sem=m_in.m1_i1;
  if (sem_array[sem] != NULL){
    while (sem_array[sem]->val < 0){
      do_semup(sem);
    }
    free(sem_array[sem]);
    sem_array[sem] = NULL;
  }
  printf("sem: %d\n",sem);
  return 0;
}
