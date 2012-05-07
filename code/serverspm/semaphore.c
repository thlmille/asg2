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

PRIVATE sem **sem_array;
PRIVATE int first_flag;

PRIVATE void check_first () {
  if (first_flag != 1) {
    first_flag = 1;
    sem_array = calloc (101, sizeof(sem *));
  }
  return;
}

//memset(&sem_array,NULL,100*sizeof(sem));


PUBLIC int do_seminit(){
  /* Call check_first to see if we need to initialize */
  /*    semaphore array */
  check_first ();

  int in_id = m_in.m1_i1; //first value to pass in
  int value = m_in.m1_i2; //second value to pass in

  printf("sem : %d, value: %d\n",in_id,value);

  /* Check if passed in id number is within range */
  if (in_id > 1000 || in_id < -1000){
    return EINVAL;
  }

  /* Assign new semaphore slot in the array */
  if (in_id == 0){
    int i;
    for(i = 1; i<101; i++){
      if (sem_array[i] == NULL){
	in_id = i;
      }
    }
    /* If no slot in the array is found, return eagain */
    if (in_id == 0){
      return EAGAIN;
    }
  }

  /* Initialize new semaphore */
  if (sem_array[in_id] == NULL){
    sem *a = malloc (sizeof (struct sem*));
    a->id = in_id;
    a->val = value;
    memset(&a->proc,0,100*sizeof(int));
    memcpy(sem_array[in_id],&a,sizeof(sem*));
  } 
  /* Return eexist if slot is already used */
  else {
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
