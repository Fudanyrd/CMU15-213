#pragma once
#ifndef CSAPP_H
#define CSAPP_H

#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**< file descriptor for stdin, stdout, stderr */
#ifndef STDIN_FILENO
#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#endif // STDIN_FILENO

/**< file descriptor type */
typedef int file_des_t;
/**< signal handler type */
typedef void (* sighandler_t)(int);

/**< wrapper of malloc */
void *Malloc(size_t bytes);

/**< wrapper of fork */
pid_t Fork();
/**< wrapper of kill */
void Kill(pid_t pid, int sig);
/**< wrapper of waitpid */
pid_t Waitpid(pid_t pid, int *stat, int options);

/**< wrapper of open function */
file_des_t Open(char *filename, int flags, mode_t mode);
/**< wrapper of close function */
void Close(file_des_t fd);

/**< wrapper of read function */
ssize_t Read(int fd, void *buf, size_t n);
/**< wrapper of write function */
ssize_t Write(int fd, const void *buf, size_t n);

/**
 * wrapper of sigprocmask
 *
 * @param how can be one of the following: `SIG_BLOCK`, `SIG_UNBLOCK`,
 * `SIG_SETMASK`. `SIG_BLOCK`: blocked | set; `SIG_UNBLOCK`: blocked & ~set;
 * `SIG_SETMASK`: set
 */
int Sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
/**< wrapper of sigemptyset */
int Sigemptyset(sigset_t *set);
/**< wrapper of sigfillset */
int Sigfillset(sigset_t *set);
/**< wrapper of sigaddset */
int Sigaddset(sigset_t *set, int signum);
/**< wrapper of sigdelset */
int Sigdelset(sigset_t *set, int signum);

/**< wrapper of signal (sighandler_t = int (*)(void)) */
sighandler_t Signal(int signum, sighandler_t handler);

/**< wrapper of stats */
void Stat(const char *filename, struct stat *st);
void Fstat(int fd, struct stat *st);

/**< Wrapper of directory */
DIR *Opendir(const char *fname);
void Closedir(DIR *dir);

/**< Wrapper of dup2 */
file_des_t Dup2(file_des_t oldfd, file_des_t newfd);

#endif // CSAPP_H
