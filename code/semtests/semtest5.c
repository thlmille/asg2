#include <stdio.h>

int main (int argc, char **argv) {
   int a = seminit (1, 1);
   printf("%d\n", a);
   int b = semdown (1);
   printf("%d\n", b);
}
