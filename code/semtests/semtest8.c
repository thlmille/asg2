#include <stdio.h>

int main (int argc, char **argv) {
  seminit(1, 0);
  semfree (1);
  seminit(1, 0);
  semfree (1);
  seminit(1, 0);
  semfree (1);
  seminit(1, 0);
  semfree (1);
}
