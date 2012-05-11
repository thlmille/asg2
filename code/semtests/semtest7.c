#include <stdio.h>

int main (int argc, char **argv) {
  printf("semfree**********\n");
  semfree (5);
  semfree (5);
  semfree (5);
  semfree (5);
  semfree (5);
  printf("semdown***********\n");
  semdown (5);
  printf("semup*************\n");
  semup (5);
  printf("semvalue**********\n");
  semvalue (5);
}
