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
/** wrapper of waitpid */
pid_t Waitpid(pid_t pid, int *stat, int options) {
  pid_t res;
  if ((res = waitpid(pid, stat, options)) == -1) {
    fprintf(stderr, "in function Waitpid: failure\n");
    exit(1);
  }

  return res;
}
/** wrapper of kill */
void Kill(pid_t pid, int sig) {
  if (kill(pid, sig) == -1) {
    fprintf(stderr, "in function Kill: failure\n");
    exit(1);
  }
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

/** wrapper of sigprocmask */
int Sigprocmask(int how, const sigset_t *set, sigset_t *oldset) {
  int ret = sigprocmask(how, set, oldset);
  if (ret == -1) {
    fprintf(stderr, "in function Sigprocmask: failure\n");
    exit(1);
  }

  return ret;
}
/** wrapper of sigemptyset */
int Sigemptyset(sigset_t *set) {
  int ret = sigemptyset(set);
  if (ret == -1) {
    fprintf(stderr, "in function Sigemptyset: failure\n");
    exit(1);
  }

  return ret;
}
/** wrapper of sigfillset */
int Sigfillset(sigset_t *set) {
  int ret = sigfillset(set);
  if (ret == -1) {
    fprintf(stderr, "in function Sigfillset: failure\n");
    exit(1);
  }

  return ret;
}
/** wrapper of sigaddset */
int Sigaddset(sigset_t *set, int signum) {
  int ret = sigaddset(set, signum);
  if (ret == -1) {
    fprintf(stderr, "in funtion Sigaddset: failure\n");
    exit(1);
  }

  return ret;
}
/** wrapper of sigdelset */
int Sigdelset(sigset_t *set, int signum) {
  int ret = sigdelset(set, signum);
  if (ret == -1) {
    fprintf(stderr, "in function Sigdelset: failure\n");
    exit(1);
  }

  return ret;
}

sighandler_t Signal(int signum, sighandler_t handler) {
  handler = signal(signum, handler);

  if (handler == SIG_ERR) {
    fprintf(stderr, "in funtion Signal: failure\n");
    exit(1);
  }

  return handler;
}

void Stat(const char *filename, struct stat *st) {
  int ret = stat(filename, st);
  if (ret == -1) {
    fprintf(stderr, "in funtion Stat: failure\n");
    exit(1);
  }
}
void Fstat(int fd, struct stat *st) {
  int ret = fstat(fd, st);
  if (ret == -1) {
    fprintf(stderr, "in funtion Fstat: failure\n");
    exit(1);
  }
}

DIR *Opendir(const char *fname) {
  DIR *ret = opendir(fname);
  if (ret == NULL) {
    fprintf(stderr, "in funtion Opendir: failure\n");
    exit(1);
  }
  return ret;
}
void Closedir(DIR *dir) {
  int ret = closedir(dir);
  if (ret == -1) {
    fprintf(stderr, "in function Closedir: failure\n");
    exit(1);
  }
}

file_des_t Dup2(file_des_t oldfd, file_des_t newfd) {
  file_des_t ret = dup2(oldfd, newfd);
  if (ret == -1) {
    fprintf(stderr, "in function Dup2: failure\n");
    exit(1);
  }

  return ret;
}
