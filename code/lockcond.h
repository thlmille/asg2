/* lockcond.h, user level library for locks and condition variables */

typedef int semaphore;

struct lock {
  semaphore mutex;
  semaphore next;
  int next_count;
};

typedef struct lock * lock_ptr;

struct cond {
  lock_ptr the_lock;
  semaphore cond_sem;
  int sem_count;
};

typedef struct cond * cond_ptr;

void lock_init (lock_ptr l);

void lock_acquire (lock_ptr l);

void lock_release (lock_ptr l);

void cond_init (cond_ptr cnd, lock_ptr l);

void cond_wait (cond_ptr cnd);

void cond_signal (cond_ptr cnd);





