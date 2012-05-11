#include <stdio.h>

int main (int argc, char **argv) {
  int foo = seminit(1,0);
  printf ("semval = %d\n", foo);
  int bar = seminit(1,0);
  if (bar != 1) printf("bar rejected\n");
  semfree(1);
  return 0;
}
