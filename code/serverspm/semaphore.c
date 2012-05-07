#include "pm.h"
#include <sys/queue.h>
#include <errno.h>
#include <lib.h>
#include <stdio.h>
#include <stdlib.h>
//#include <string.h>

#include "param.h"
#include "mproc.h"
#include "semtable.h"


struct sem{
  int id;
  int val;
  int proc[1000];
};

PRIVATE semtable_ref main_table;
PRIVATE int first_flag;

/* Get a string from a num so I can reuse the hashing function
   from the stringtable I made for cmps104 */
PRIVATE char *strnum (int num) {
  int hold = num;
  int size = 0;
  while (hold != 0) {
    size++;
    hold = hold / 10;
  }
  char *digits = calloc (size+1, sizeof (char));
  char *itor = digits + size - 1;
  int curr_dig;
  while (num != 0) {
    curr_dig = num % 10;
    *itor = (char) (curr_dig + 48);
    itor--;
    num = num / 10;
  }
  return digits;
}

PRIVATE sem *fetch_sem (int id_num) {
  return get_sem (main_table, strnum (id_num));
}

/* Function to initialize arrays if seminit is being called for the */
/* first time */
PRIVATE void check_first () {
  if (first_flag != 1) {
    first_flag = 1;
    main_table = new_semtable ();
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
  int ret_id;

  printf("sem : %d, value: %d\n",in_id,value);

  /* Check if passed in values are acceptable */
  if (value > 1000 || value < -1000 || in_id < 0){
    return EINVAL;
  }

  /* Assign new semaphore slot in the array */

  /* First case where user does not give an id number */
  if (in_id == 0){
    /* Try intern_stringtable with different random values until */
    /*       we find an empty slot */
    int rand_id = 1; // Not actually random
    for (;;) {
      /* The loading factor of the table is .45, so the likely-hood */
      /*    of this loop repeating more than a few times is slim */
      sem *new_sem = malloc (sizeof(sem));
      new_sem->id = rand_id;
      new_sem->val = value;
      if (intern_semtable (main_table, strnum(rand_id), new_sem))
	break;
      free (new_sem);
      ++rand_id;
    }
    ret_id = rand_id;
  }
  /*  Second case where user provides an id number */
  else {
    sem *new_sem = malloc (sizeof(sem));
    new_sem->id = in_id;
    new_sem->val = value;
    if (!intern_semtable (main_table, strnum(in_id), new_sem))
      return EEXIST;
    ret_id = in_id;
  }
  return ret_id;
}

PUBLIC int do_semvalue(){
  int id_num = m_in.m1_i1;
  sem *the_sem = fetch_sem (id_num);
  if (the_sem == NULL){
    return 0x8000000;
  } else {
    printf("returning : %d\n" ,the_sem->val);
    return the_sem->val;
  }
}

PUBLIC int do_semup(){
  int id_num = m_in.m1_i1;
  sem *the_sem = fetch_sem (id_num);
  if (the_sem != NULL){
    the_sem->val += 1;
    if (the_sem->val > 1000){
      return EOVERFLOW;
    }
  } else {
    return 1;
  }
  if (the_sem->val <= 0){
    int process = the_sem->proc[0];
    mp[process].mp_flags |= REPLY;

    // Commented out cause I dont know what this shit does
    //memcpy(&sem_array,&sem_array+1,99*sizeof(int));
    //do_(); //this is where we wake up waiting proc
  }
  //printf("sem: %d\n",sem);
  return 0;
}

PUBLIC int do_semdown(){
  int id_num = m_in.m1_i1;
  sem *the_sem = fetch_sem(id_num);
  if (the_sem != NULL){
    the_sem->val -= 1;
    if (the_sem->val < 1000){
      return EOVERFLOW;
    }
  } else {
    return 1;
  }
  if (the_sem->val < 0){
    mp->mp_flags |= PAUSE;
    int i = 0;
    while(i<100){
      if (the_sem->proc[i] == 0){
	the_sem->proc[i] = mp->mp_pid;
	i = 101;
      }
    }
  }
  return 0;
}

PUBLIC int do_semfree(){
  int id_num = m_in.m1_i1;
  sem *the_sem = fetch_sem(id_num);
  if (the_sem == NULL) return 0;
  while (the_sem->val < 0) {
    do_semup (id_num);
  }
  delete_node (main_table, strnum(id_num));
  return 1;
}
