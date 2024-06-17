#pragma once
/**
 * Dependencies: 
 * 
 * file_util.c
 */

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/** file descriptor type */
typedef int file_des_t;

/** wrapper of malloc */
void *Malloc(size_t bytes);

/** wrapper of fork */
pid_t Fork();

/** wrapper of open function */
file_des_t Open(char *filename, int flags, mode_t mode);
/** wrapper of close function */
void Close(file_des_t fd);

/** wrapper of read function */
ssize_t Read(int fd, void *buf, size_t n);
/** wrapper of write function */
ssize_t Write(int fd, const void *buf, size_t n);
