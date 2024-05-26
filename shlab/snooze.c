#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

typedef void (*sighandler_t)(int);

void handler(int pid) {
}

/**
 * Example usage:
 * ./snooze 4
 * <ctrl+c>
 * Slept 2 of 4 seconds.
 */
int main(int argc, char **argv) {
  unsigned int secs;
  unsigned int left;

  if (argc != 2) {
    fprintf(stderr, "usage snooze <secs>\n");
    _exit(1);
  }

  // sleep seconds
  secs = atoi(argv[1]);

  sighandler_t h = signal(SIGINT, handler);
  if (h == SIG_ERR) {
    printf("oops, fail\n");
    _exit(0);
  }
  if (h == handler) {
    handler(2);
    _exit(0);
  }
  left = sleep(secs);
  printf("\nSlept for %u of %u seconds.\n", secs - left, secs);

  return 0;
}