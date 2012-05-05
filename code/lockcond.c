#include <stdio.h>
#include <stdlib.h>
#include "lockcond.h"

void lock_init (lock_ptr l) {
  l->mutex = seminit (0, 1);
  l->next = seminit (0, 0);
  l->next_count = 0;
}

void lock_acquire (lock_ptr l) {
  semdown (l->mutex);
}

void lock_release (lock_ptr l) {
  if (l->next_count > 0) {
    semup (l->next);
  } else {
    semup (l->mutex);
  }
}

void cond_init (cond_ptr cnd, lock_ptr l) {
  cnd->the_lock = l;
  cnd->cond_sem = seminit (0, 0);
  cnd->sem_count = 0;
}

void cond_wait (cond_ptr cnd) {
  cnd->sem_count++;
  if (cnd->the_lock->next_count > 0) {
    semup (cnd->the_lock->next);
  } else {
    semup (cnd->the_lock->mutex);
  }
  semdown (cnd->cond_sem);
  cnd->sem_count--;
}

void cond_signal (cond_ptr cnd) {
  if (cnd->sem_count > 0) {
    cnd->the_lock->next_count++;
    semup (cnd->cond_sem);
    semdown (cnd->the_lock->next);
    cnd->the_lock->next_count--;
  }
}

