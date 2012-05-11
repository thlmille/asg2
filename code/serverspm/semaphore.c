#include "pm.h"
#include <sys/queue.h>
#include <errno.h>
#include <lib.h>
#include <stdio.h>
#include <stdlib.h>

#include "param.h"
#include "mproc.h"



typedef struct sem{
  int id;
  int val;
  int proc[1000];
} sem;

PRIVATE sem sem_array[100];
int debug = 1;
int nsems = 0;

/* Array of numbers to use when we need to assign an id */
int used_nums[101];

PRIVATE void debug_message (int flag, char *str) {
  if (flag) {
    printf ("%s\n", str);
  }
}

PRIVATE void initialize () {
  debug_message(debug, "in initialize");
  int i;
  for (i = 0; i < 100; ++i) {
    sem_array[i].id = 0;
    used_nums[i] = 0;
  }
  used_nums[i] = 0;
}

PUBLIC int do_seminit () {
  int user_id = m_in.m1_i1;
  int value = m_in.m1_i2;

  if (nsems == 0) initialize();

  if (debug) {
    printf ("in seminit\n");
    printf ("user_id = %d\n", user_id);
    printf ("value = %d\n", value);
    printf ("nsems = %d\n", nsems);
  }

  if (nsems == 99) {
    debug_message (debug, "semaphore limit exceeded");
    return -1 * EAGAIN;
  }

  if (user_id < 0) {
    debug_message (debug, "id less than 0");
    return -1 * EINVAL;
  }
  
  if (value > 1000 || value < -1000) {
    debug_message (debug, "value out of range");
    return -1 * EINVAL;
  }

  /* If given ID is 0, assign an id number */
  if (user_id == 0) {
    /* Find open slot in used_nums array */
    debug_message (debug, "assigning value");
    int i;
    for (i = 1; i < 101; ++i) {
      if (!used_nums[i]) {
	user_id = i;
	used_nums[i] = 1;
	break;
      }
    }
  }

  /* Check if semaphore already exists, if not, create it */
  debug_message (debug, "checking if semaphore exists");
  int i;
  for (i = 0; i < 100; ++i) {
    if (user_id == sem_array[i].id) {
      debug_message (debug, "semaphore already exists");
      return -1 * EEXIST;
    }
  }
  /* Update used_nums array if user_id is less than 101 */
  if (user_id <= 100) {
    used_nums[user_id] = 1;
  }
  /* Find first open slot in array and initialize that semaphore */
  int open;
  for (open = 0; open < 100; ++open) {
    if (sem_array[open].id == 0) break;
  }
  /* This should never happen */
  if (open == 100) {
    debug_message (debug, "shit is fucked");
    return -1 *EAGAIN;
  }
  /* Initialize semaphore */
  debug_message (debug, "initialize semaphore");
  sem_array[open].id = user_id;
  sem_array[open].val = value;
  int j;
  for (j = 0; j < 1000; ++j) {
    sem_array[open].proc[j] = 0;
  }

  nsems++;
  return user_id;
}

PUBLIC int do_semup () {
  int sem_id = m_in.m1_i1;
  if (debug) {
    printf("in semup\n sem_id = %d\n", sem_id);
  }
  /* Find the semaphore, error if it doesn't exist */
  int i;
  for (i = 0; i < 100; ++i) {
    if (sem_array[i].id == sem_id) break;
  }
  if (i == 100) {
    debug_message (debug, "semaphore does not exist\n");
    return -1 * EEXIST;
  }
  int sem_index = i;
  /* Make sure value is in range */
  if (sem_array[sem_index].val > 1000) {
    debug_message (debug, "value > 1000");
    return EOVERFLOW;
  }
  /* If value is less than 0, run the first process waiting */
  return 0;
}
PUBLIC int do_semdown () {
  return 0;
}
PUBLIC int do_semvalue () {
  int sem_id = m_in.m1_i1;
  if (debug) {
    printf("in semvalue\n sem_id = %d\n", sem_id);
  }
  /* Find the semaphore, error if it doesn't exist */
  int i;
  for (i = 0; i < 100; ++i) {
    if (sem_array[i].id == sem_id) break;
  }
  if (i == 100) {
    debug_message (debug, "semaphore does not exist\n");
    return -1 * EEXIST;
  }
  return sem_array[i].val;
}

PUBLIC int do_semfree () {
  int sem_id = m_in.m1_i1;
  if (debug) {
    printf ("in semfree\n sem_id = %d\n", sem_id);
  }
  /* Find semaphore */
  int i;
  for (i = 0; i < 100; ++i) {
    if (sem_array[i].id == sem_id) break;
  }
  if (i == 100) {
    debug_message (debug, "semaphore does not exist");
    return -1 * EEXIST;
  }
  /* Set id to zero to mark it free */
  /* placeholder for calling semup once we have that function */
  sem_array[i].id = 0;
  nsems--;
  if (sem_id < 101) {
    used_nums[sem_id] = 0;
  }
  return 1;
}

