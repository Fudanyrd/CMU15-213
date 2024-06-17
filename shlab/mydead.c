#include <stdio.h>

/** there's certainly no way to terminate me! */
int main(int argc, char **argv) {
  printf("You cannot terminate me! I'm evil!\n");
  // but is it really the case?!
  while (1) {}
  return 0;
}