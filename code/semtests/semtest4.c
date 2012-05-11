#include <stdio.h>

int main (int argc, char **argv) {
  int a = seminit (0, 4);
  int b = seminit (0, 5);
  printf ("a = %d\n", semvalue(a));
  printf ("b = %d\n", semvalue(b));
  int c = semfree (a);
  int d = semfree (b);
  printf ("%d\n", semvalue(a+b));

}
