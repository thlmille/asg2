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

PRIVATE semtable_ref main_table;
PRIVATE int used_nums[2001];
PRIVATE int first_flag;

/* Get a string from a num so I can reuse the hashing function
   from the stringtable I made for cmps104, put an 'a' in front
   of negative numbers to account for negative ids */
PRIVATE char *strnum (int num) {
  int hold = num;
  int size = 0;
  while (hold != 0) {
    size++;
    hold = hold / 10;
  }
  int extra_slots = 1; /* 1 for positive number (null plug) */
  if (num < 0) extra_slots = 2; /* add room for 'a' at beginning of */
                                   /* negative numbers */
  char *digits = calloc (size+extra_slots, sizeof (char));
  char *itor = digits + size - 1;
  int curr_dig;
  while (num != 0) {
    curr_dig = num % 10;
    *itor = (char) (curr_dig + 48);
    itor--;
    num = num / 10;
  }
  /* add a at beginning slot if number is negative */
  if (num < 0) {
    itor--;
    *itor = 'a';
  }
  return digits;
}

/* Function to initialize arrays if seminit is being called for the */
/* first time */
PRIVATE void check_first () {
  if (first_flag != 1) {
    first_flag = 1;
    sem_table = new_semtable ();
    int i;
    for (i = 0; i < 2001; ++i) {
      used_nums[i] = 1;
    }
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

  /* First case where user does not give an id number */
  if (in_id == 0){
    /* Find first open number */
    int i;
    for (i = 1; i < 2001; ++i) {
      if (used_nums[i]) break;
    }

    /* Return error if all numbers are used */
    if (i == 2001) return EAGAIN;

    /* Initialize new semaphore and put into hashtable */
    int id_num = i - 1000;
    sem *new_sem = malloc (sizeof (struct sem*));
    new_sem->id = id_num;
    new_sem->val = value; 
    intern_semtable (main_table, strnum(new_sem->id), new_sem);
  }

  /*  Second case where user provides an id number */


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
