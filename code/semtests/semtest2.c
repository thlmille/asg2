#include <stdio.h>

int main (int argc, char **argv) {
  int i;
  for (i = 1; i < 40; ++i) {
    printf("%d\n", seminit (i, 0));
  }
  // Free those 1000 semaphores
  for (i = 1; i < 40; ++i) {
    printf("%d\n", semfree(i));
  }
}
