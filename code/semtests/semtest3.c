#include <stdio.h>

int main (int argc, char **argv) {
  int a = seminit (5, 0);
  int b = seminit (5, 0);
  int c = semfree (5);
  int d = seminit (5, 0);
  printf ("a = %d\n", a);
  printf ("b = %d\n", b);
  printf ("c = %d\n", c);
  printf ("d = %d\n", d);
}
