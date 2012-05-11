#include <stdio>

int main (int argc, char **argv) {
  int a = seminit (0, 0);
  int b = seminit (0, 0);
  printf("a = %d\n", a);
  printf("b = %d\n", b);
  semfree(a);
  semfree(b);
}
