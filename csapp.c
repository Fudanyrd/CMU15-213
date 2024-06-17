#include "csapp.h"

/** wrapper of malloc */
void *Malloc(size_t bytes) {
  void *ptr = malloc(bytes);
  if (ptr == NULL) {
    fprintf(stderr, "in function Malloc: failure\n");
    exit(1);
  }

  return ptr;
}

/** wrapper of fork */
pid_t Fork() {
  pid_t res = fork();
  if (res == -1) {
    fprintf(stderr, "in function Fork: failure\n");
    exit(1);
  }

  return res;
}

/** wrapper of open function */
file_des_t Open(char *filename, int flags, mode_t mode) {
  file_des_t res = open(filename, flags, mode);
  if (res == -1) {
    fprintf(stderr, "in function Open: failure\n");
    exit(1);
  }
  return res;
}

/** wrapper of close function */
void Close(file_des_t fd) {
  int res = close(fd);
  if (res == -1) {
    fprintf(stderr, "in function Close: failure\n");
    exit(1);
  }
}

/** wrapper of read function */
ssize_t Read(int fd, void *buf, size_t n) {
  ssize_t bytes = read(fd, buf, n);

  if (bytes == -1L) {
    fprintf(stderr, "in function Read: failure\n");
    exit(1);
  }

  return bytes;
}

/** wrapper of write function */
ssize_t Write(int fd, const void *buf, size_t n) {
  ssize_t bytes = write(fd, buf, n);

  if (bytes == -1L) {
    fprintf(stderr, "in function Write: failure\n");
    exit(1);
  }

  return bytes;
}
