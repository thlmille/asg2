#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include "lockcond.h"


/* Test code found on the internet */

char buffer[SIZE];
int count = 0, head = 0, tail = 0;
struct lock l;
lock_init(&l);

void put(char c) {
  lock_acquire(&l);
  count++;
  buffer[head] = c;
  sleep(1);
  head++;
  if (head == SIZE) {
    head = 0;
  }
  lock_release(&l);
}

char get() {
  char c;
  lock_acquire(&l);
  count--;
  c = buffer[tail];
  tail++;
  if (tail == SIZE) {
    tail = 0;
  }
  lock_release(&l);
  return c;
}

int main (int argc, char **argv) {
  fork();
  put('c');
  get();
}
