#include "pm.h"
#include <sys/queue.h>
#include <errno.h>
#include <lib.h>
#include <stdio.h>
#include <stdlib.h>

#include "param.h"
#include "mproc.h"

#define DEBUG 1

#ifdef DEBUG
#define PRINT_DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define PRINT_DEBUG(...)
#endif

typedef struct sem{
  int id;
  int val;
  int proc[1000];
} sem;

PRIVATE sem sem_array[100];
int nsems = 0;

/* Array of numbers to use when we need to assign an id */
int used_nums[101];


PRIVATE void initialize () {
  PRINT_DEBUG("in initialize\n");
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

  PRINT_DEBUG ("in seminit\n");
  PRINT_DEBUG ("user_id = %d\n", user_id);
  PRINT_DEBUG ("value = %d\n", value);
  PRINT_DEBUG ("nsems = %d\n", nsems);

  if (nsems == 99) {
    PRINT_DEBUG("semaphore limit exceeded\n");
    return -1 * EAGAIN;
  }

  if (user_id < 0) {
    PRINT_DEBUG("id less than 0\n");
    return -1 * EINVAL;
  }
  
  if (value > 1000 || value < -1000) {
    PRINT_DEBUG("value out of range\n");
    return -1 * EINVAL;
  }

  /* If given ID is 0, assign an id number */
  if (user_id == 0) {
    /* Find open slot in used_nums array */
    PRINT_DEBUG("assigning value\n");
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
  PRINT_DEBUG("checking if semaphore exists\n");
  int i;
  for (i = 0; i < 100; ++i) {
    if (user_id == sem_array[i].id) {
      PRINT_DEBUG("semaphore already exists\n");
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
    PRINT_DEBUG("shit is fucked\n");
    return -1 *EAGAIN;
  }
  /* Initialize semaphore */
  PRINT_DEBUG("initialize semaphore\n");
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
  PRINT_DEBUG("in semup\n sem_id = %d\n", sem_id);
  /* Find the semaphore, error if it doesn't exist */
  int i;
  for (i = 0; i < 100; ++i) {
    if (sem_array[i].id == sem_id) break;
  }
  if (i == 100) {
    PRINT_DEBUG("semaphore does not exist\n");
    return -1 * EEXIST;
  }
  int sem_index = i;
  /* Make sure value is in range */
  if (sem_array[sem_index].val < -1000) {
    PRINT_DEBUG("value < -1000\n");
    return EOVERFLOW;
  }
  /* If value is less than 0, run the first process waiting */
  if (sem_array[sem_index].val < 0) {
    PRINT_DEBUG("value less than 0\n");
    sem_array[sem_index].val++;
    /* Take first process, move other processes over */
    int process = sem_array[sem_index].proc[0];
    int j;
    for (j = 1; j < 1000; ++j) {
      sem_array[sem_index].proc[j-1] = sem_array[sem_index].proc[j];
    }
    PRINT_DEBUG("waking process %d\n", process);
    setreply (process, OK);
  }
  return 0;
}
PUBLIC int do_semdown () {
  int sem_id = m_in.m1_i1;
  PRINT_DEBUG("in semvalue\nsem_id = %d\n", sem_id);
  /* Find the semaphore, error if it doesn't exist */
  int i;
  for (i = 0; i < 100; ++i) {
    if (sem_array[i].id == sem_id) break;
  }
  if (i == 100) {
    PRINT_DEBUG("semaphore does not exist\n");
    return -1 * EEXIST;
  }
  int sem_index = i;
  /* Make sure value is in range */
  if (sem_array[sem_index].val < -1000) {
    PRINT_DEBUG ("value = %d\n", sem_array[sem_index].val);
    PRINT_DEBUG("value < -1000\n");
    return EOVERFLOW;
  }
  /* if value is greater than 0, decrement value */
  if (sem_array[sem_index].val > 0) {
    sem_array[sem_index].val--;
  }
  /* if value is less than 0, decrement and add process to queue */
  else {
    sem_array[sem_index].val--;
    int j;
    for (j = 0; j < 1000; ++j) {
      if (sem_array[sem_index].proc[j] == 0) {
	    PRINT_DEBUG ("process %d added to queue", who_p);
	    sem_array[sem_index].proc[j] = who_p;
	    break;
      }
    }
    if (DEBUG) {
      PRINT_DEBUG("process list\n");
      for (i = 0; i < 5; ++i) {
	    printf ("%d: %d\n", i, sem_array[sem_index].proc[i]);
      }
    }
    return SUSPEND;
  }
  return 0;
}
PUBLIC int do_semvalue () {
  int sem_id = m_in.m1_i1;
  if (DEBUG) {
    printf("in semvalue\n sem_id = %d\n", sem_id);
  }
  /* Find the semaphore, error if it doesn't exist */
  int i;
  for (i = 0; i < 100; ++i) {
    if (sem_array[i].id == sem_id) break;
  }
  if (i == 100) {
    PRINT_DEBUG("semaphore does not exist\n");
    return -1 * EEXIST;
  }
  return sem_array[i].val;
}

PUBLIC int do_semfree () {
  int sem_id = m_in.m1_i1;
  PRINT_DEBUG("in semfree\n sem_id = %d\n", sem_id);
  /* Find semaphore */
  int i;
  for (i = 0; i < 100; ++i) {
    if (sem_array[i].id == sem_id) break;
  }
  if (i == 100) {
    PRINT_DEBUG("semaphore does not exist\n");
    return -1 * EEXIST;
  }
  /* semup until all processes are freed */
  while (sem_array[i].val < 0) do_semup (sem_id);
  /* Set id to zero to mark it free */
  sem_array[i].id = 0;
  nsems--;
  if (sem_id < 101) {
    used_nums[sem_id] = 0;
  }
  return 0;
}

