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

ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n) {
  /**< check invalid argument */
  assert(rp != NULL && usrbuf != NULL);
  int cnt;

  while (rp->rio_cnt_ <= 0) {
    /**< buffer is empty, should refill it */
    rp->rio_cnt_ = read(rp->rio_fd_, rp->rio_buf_, sizeof(rp->rio_buf_));

    if (rp->rio_cnt_ < 0) {
      if (errno != EINTR) {
        /**< Not sighandler return interruption */
        return -1;
      }
    } else {
      if (rp->rio_cnt_ == 0) {
        /**< EOF */
        return 0;
      } else {
        /**< reset buffer ptr */
        rp->rio_bufptr_ = rp->rio_buf_;
      }
    } 
  }

  /**< copy min(n, rp->rio_cnt_) bytes from internal buf to user buf */
  cnt = n < rp->rio_cnt_ ? n : rp->rio_cnt_;
  memcpy(usrbuf, rp->rio_bufptr_, cnt);
  rp->rio_bufptr_ += cnt;
  rp->rio_cnt_ -= cnt;

  return cnt;
}
ssize_t Rio_read(rio_t *rp, char *usrbuf, size_t n) {
  /**< check invalid argument */
  assert(rp != NULL && usrbuf != NULL);
  int cnt;

  while (rp->rio_cnt_ <= 0) {
    /**< buffer is empty, should refill it */
    rp->rio_cnt_ = read(rp->rio_fd_, rp->rio_buf_, sizeof(rp->rio_buf_));

    if (rp->rio_cnt_ < 0) {
      if (errno != EINTR) {
        /**< Not sighandler return interruption */
        fprintf(stderr, "in function Rio_read: failure\n");
        exit(1);
      }
    } else {
      if (rp->rio_cnt_ == 0) {
        /**< EOF */
        return 0;
      } else {
        /**< reset buffer ptr */
        rp->rio_bufptr_ = rp->rio_buf_;
      }
    } 
  }

  /**< copy min(n, rp->rio_cnt_) bytes from internal buf to user buf */
  cnt = n < rp->rio_cnt_ ? n : rp->rio_cnt_;
  memcpy(usrbuf, rp->rio_bufptr_, cnt);
  rp->rio_bufptr_ += cnt;
  rp->rio_cnt_ -= cnt;

  return cnt;
}

ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n) {
  size_t nleft = n;
  ssize_t nread;
  char *bufp = usrbuf;

  while (nleft > 0) {
    nread = rio_read(rp, bufp, nleft);
    if (nread < 0) {
      /**< failure */
      return -1;
    }
    else {
      if (nread == 0) {
        break;
      }
    }

    nleft -= nread;
    bufp += nread;
  }

  return (n - nleft);
}
ssize_t Rio_readnb(rio_t *rp, void *usrbuf, size_t n) {
  size_t nleft = n;
  ssize_t nread;
  char *bufp = usrbuf;

  while (nleft > 0) {
    nread = Rio_read(rp, bufp, nleft);
    if (nread == 0) {
      break;
    }

    nleft -= nread;
    bufp += nread;
  }

  return (n - nleft);
}
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen) {
  int n;
  int rc;
  char c;
  char *bufp = usrbuf;

  for (n = 0; n < maxlen; ++n) {
    rc = rio_read(rp, &c, 1);
    if (rc == 1) {
      *bufp = c; ++bufp;
      if (c == '\n') {
        /**< terminates */
        n++;
        break;
      }
    } else {
      if (rc == 0) {
        /**< EOF, no or some data is read */
        break;
      } else {
        /**< Error! */
        return -1;
      }
    }
  }

  *bufp = 0;
  return n;
}
ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen) {
  int n;
  int rc;
  char c;
  char *bufp = usrbuf;

  for (n = 0; n < maxlen; ++n) {
    rc = rio_read(rp, &c, 1);
    if (rc == 1) {
      *bufp = c; ++bufp;
      if (c == '\n') {
        /**< terminates */
        n++;
        break;
      }
    } else {
      if (rc == 0) {
        /**< EOF, no or some data is read */
        break;
      } else {
        /**< Error! */
        fprintf(stderr, "in function Rio_readlineb: failure\n");
        exit(1);
      }
    }
  }

  *bufp = 0;
  return n;
}

ssize_t rio_readn(int fd, void *usrbuf, size_t n) {
  size_t nleft = n;
  ssize_t nread;
  char *bufp = usrbuf;

  while (nleft > 0) {
    nread = read(fd, bufp, nleft);
    if (nread < 0) {
      if (errno == EINTR) {
        /**< Interrupted by sighandler return */
        continue;
      } else {
        /**< read failure */
        return -1;
      }
    } else {
      if (nread == 0) {
        /**< EOF */
        break;
      }

      nleft -= nread;
      bufp += nread;
    }
  }

  return (n - nleft);
}
ssize_t Rio_readn(int fd, void *usrbuf, size_t n) {
  size_t nleft = n;
  ssize_t nread;
  char *bufp = usrbuf;

  while (nleft > 0) {
    nread = read(fd, bufp, nleft);
    if (nread < 0) {
      if (errno == EINTR) {
        /**< Interrupted by sighandler return */
        continue;
      } else {
        /**< read failure, terminate */
        fprintf(stderr, "in function Rio_readn: failure\n");
        return -1;
      }
    } else {
      if (nread == 0) {
        /**< EOF */
        break;
      }

      nleft -= nread;
      bufp += nread;
    }
  }

  return (n - nleft);
}

ssize_t rio_writen(int fd, void *usrbuf, size_t n) {
  size_t nleft = n;
  ssize_t nwritten;
  char *bufp = usrbuf;

  while (nleft > 0) {
    nwritten = write(fd, usrbuf, nleft);
    if (nwritten < 0) {
      if (errno == EINTR) {
        /**< Interupted by sighandler return */
        continue;
      } else {
        /**< write failure */
        return -1;
      }
    }
    nleft -= nwritten;
    bufp += nwritten;
  }

  return n;
}
ssize_t Rio_writen(int fd, void *usrbuf, size_t n) {
  size_t nleft = n;
  ssize_t nwritten;
  char *bufp = usrbuf;

  while (nleft > 0) {
    nwritten = write(fd, usrbuf, nleft);
    if (nwritten < 0) {
      if (errno == EINTR) {
        /**< Interupted by sighandler return */
        continue;
      } else {
        /**< write failure */
        fprintf(stderr, "in function Rio_writen: failure\n");
        exit(1);
      }
    }
    nleft -= nwritten;
    bufp += nwritten;
  }

  return n;
}

sock_des_t Socket(int domain, int type, int protocol) {
  sock_des_t ret = socket(domain, type, protocol);
  assert(ret != -1);
  return ret;
}
void Connect(sock_des_t clientfd, const struct sockaddr *addr,
            socklen_t addrlen) {
  int ret = connect(clientfd, addr, addrlen);
  assert(ret >= 0);
}

int Bind(int sockfd, struct sockaddr *addr, socklen_t addrlen) {
  int ret = bind(sockfd, addr, addrlen);
  assert(ret != -1);
  return ret;
}
int Listen(int sockfd, int backlog) {
  int ret = listen(sockfd, backlog);
  assert(ret != -1);
  return ret;
}
sock_des_t Accept(int listenfd, struct sockaddr *addr, socklen_t *addrlen) {
  int ret = accept(listenfd, addr, addrlen);
  assert(ret != -1);
  return ret;
}

void Getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints,
                struct addrinfo **res) {

  int ret = getaddrinfo(node, service, hints, res);
  assert (ret == 0);
}
void Getnameinfo(const struct sockaddr *sa, socklen_t salen,
                char *host, size_t host_len,
                char *service, size_t servlen, int flags) {
  int ret = getnameinfo(sa, salen, host, host_len, service, servlen, flags);                  
  assert (ret == 0);
}

sock_des_t open_clientfd(char *hostname, char *port) {
  int clientfd;
  struct addrinfo hints;
  struct addrinfo *listp;
  struct addrinfo *p;

  init_addrinfo(&hints);
  /**< open a connection */
  hints.ai_socktype = SOCK_STREAM;
  /**< using numberic port arg, for connection */
  hints.ai_flags = AI_NUMERICSERV;
  hints.ai_flags |= AI_ADDRCONFIG;
  Getaddrinfo(hostname, port, &hints, &listp);

  /**< walk the list for one we can successfully connect to */
  for (p = listp; p != NULL; p = p->ai_next) {
    /**< create a socket descriptor */
    clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (clientfd < 0) {
      /**< failure! */
      continue;
    }

    if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1) {
      /**< success! */
      break;
    }

    /**< connection failure, try another */
    close(clientfd);
  }

  /**< clean up */
  freeaddrinfo(listp);
  if (p == NULL) {
    /**< all connects failed */
    return -1;
  }

  return clientfd;
}

sock_des_t open_listenfd(char *port) {
  int listenfd;
  int optval = 1;
  struct addrinfo hints;
  struct addrinfo *listp;
  struct addrinfo *p;

  /**< get a list of potential server addresses */
  init_addrinfo(&hints);
  /**< accept connections */
  hints.ai_socktype = SOCK_STREAM;
  /**< on any IP address; using port number */
  hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
  hints.ai_flags |= AI_NUMERICSERV;
  Getaddrinfo(NULL, port, &hints, &listp);

  /**< walk the list for one to bind to */
  for (p = listp; p != NULL; p = p->ai_next) {
    /**< create socket descriptor */
    listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listenfd < 0) {
      /**< socket failed! */
      continue;
    }

    /**< eliminates "address already in use" error from bind */
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    /**< bind the descriptor to the address */
    if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) {
      /**< success! */
      break;
    }
    /**< bind failed! */
    Close(listenfd);
  }

  /**< clean up */
  freeaddrinfo(listp);
  if (p == NULL) {
    /**< no address worked */
    return -1;
  }

  /**< make it a listening socket ready to accept connection requests */
  if (listen(listenfd, LISTENQ) < 0) {
    Close(listenfd);
    return -1;
  }
  
  return listenfd;
}
