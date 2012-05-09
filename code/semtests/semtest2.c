#include <stdio.h>

int main (int argc, char **argv) {
  int i;
  // Make 1000 semaphores
  for (i = 1; i < 1000; ++i) {
    printf("%d\n", seminit (i, 0));
  }
  // Free those 1000 semaphores
  for (i = 1; i < 1000; ++i) {
    printf("%d\n", semfree(i));
  }
}
