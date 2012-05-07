#include <stdio.h>

int main (int argc, char **argv) {
  int i;
  // Make 1000 random semaphores and 1000 numbered semaphores
  for (i = 1; i < 1000; ++i) {
    seminit (0, 0);
    seminit (i, 0);
  }
  // All of these semaphores should be created already, so we
  //   should not see shit printed to the terminal
  for (i = 1; i < 1000; ++i) {
    if (seminit (i, 0) != 0) {
      printf ("shit\n");
    }
  }
}
