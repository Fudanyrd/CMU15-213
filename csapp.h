#pragma once
#ifndef CSAPP_H
#define CSAPP_H

#include <arpa/inet.h>
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Macros and type definitions
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

/**< file descriptor for stdin, stdout, stderr */
#ifndef STDIN_FILENO
#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#endif // STDIN_FILENO

/**< second argument to listen */
#define LISTENQ 1024
/**< Max text line length */
#define	MAXLINE	 8192  
/**< Max I/O buffer size */
#define MAXBUF   8192  

/**< file descriptor type */
typedef int file_des_t;
/**< signal handler type */
typedef void (* sighandler_t)(int);
/**< internet address type */
typedef uint32_t in_addr_t;
/**< socket descriptor type */
typedef int sock_des_t;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Memory Utility
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

/**< wrapper of malloc */
void *Malloc(size_t bytes);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Process Utility
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

/**< wrapper of fork */
pid_t Fork();
/**< wrapper of kill */
void Kill(pid_t pid, int sig);
/**< wrapper of waitpid */
pid_t Waitpid(pid_t pid, int *stat, int options);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// System IO Utility
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

/**< wrapper of open function */
file_des_t Open(char *filename, int flags, mode_t mode);
/**< wrapper of close function */
void Close(file_des_t fd);

/**< wrapper of read function */
ssize_t Read(int fd, void *buf, size_t n);
/**< wrapper of write function */
ssize_t Write(int fd, const void *buf, size_t n);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Signal Utility
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Robust IO Utility(NOT TESTED, USE WITH CAUTION)
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

/**< wrapper of stats */
void Stat(const char *filename, struct stat *st);
void Fstat(int fd, struct stat *st);

/**< rio buffer size */
#define RIO_BUFSIZE 8192
/**< for thread-safe buffered IO */
typedef struct {
  /**< descriptor for internal buf */
  int rio_fd_;
  /**< unread bytes in internal buf */
  int rio_cnt_;
  /**< next unread byte in internal buf */
  char *rio_bufptr_;
  /**< internal buffer */
  char rio_buf_[RIO_BUFSIZE];
} rio_t;

/**< initialize rio buf by file descriptor */
static inline void rio_readinitb(rio_t *rp, int fd) {
  memset(rp->rio_buf_, 0, sizeof(rp->rio_buf_));
  rp->rio_fd_ = fd;
  rp->rio_cnt_ = 0;
  rp->rio_bufptr_ = (char *)(rp->rio_buf_);
}
/**< move n bytes from rio buffer to user buffer */
ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n);
/**< rio_read with error-handling */
ssize_t Rio_read(rio_t *rp, char *usrbuf, size_t n);
/**< If not early EOF or error, read n bytes from rio to usrbuf */
ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n);
/**< rio_readnb with error-handling */
ssize_t Rio_readnb(rio_t *rp, void *usrbuf, size_t n);
/**< read from rio until '\n' is met(copied to usrbuf) */
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
/**< rio_readlineb with error handling */
ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);

/**
 * @brief unbuffered read up to n bytes
 * @param fd file descriptor
 * @param usrbuf user should provide a buffer
 * @param n maximum of bytes read
 * @return 0 if EOF, else number of bytes read, exit on error
 */
ssize_t rio_readn(int fd, void *usrbuf, size_t n);
/**< rio_readn with error handling */
ssize_t Rio_readn(int fd, void *usrbuf, size_t n);
/**
 * @brief unbuffered write up to n bytes
 * @param fd file descriptor
 * @param usrbuf user should provide a buffer
 * @param n maximum of bytes write
 * @return 0 if EOF, else number of bytes write, exit on error
 */
ssize_t rio_writen(int fd, void *usrbuf, size_t n);
/**< rio_writen with error handling */
ssize_t Rio_writen(int fd, void *usrbuf, size_t n);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Directory(Folder) Operations
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

/**< Wrapper of directory */
DIR *Opendir(const char *fname);
void Closedir(DIR *dir);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// IO Redirection
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

/**< Wrapper of dup2 */
file_des_t Dup2(file_des_t oldfd, file_des_t newfd);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Network Programming
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

/**< convertion from host to net */
uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);

/**< convertion from net to host */
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);

/**< convert from 4 separate decimal */
static inline uint32_t
dec2addr(uint32_t first, uint32_t second, uint32_t third, uint32_t fourth) {
  return (first << 24) | (second << 16) | (third << 8) | fourth; 
}

/**< create a socket descriptor, return -1 on err */
sock_des_t Socket(int domain, int type, int protocol);
/**< client method, return -1 on error. */
void Connect(sock_des_t clientfd, const struct sockaddr *addr, socklen_t addrlen);

/**< server methods */
int Bind(int sockfd, struct sockaddr *addr, socklen_t addrlen);
int Listen(int sockfd, int backlog);
sock_des_t Accept(int listenfd, struct sockaddr *addr, socklen_t *addrlen);

/**< the addrinfo struct:
struct addrinfo {
  int              ai_flags;     // hints argument flags
  int              ai_family;    // first arg to socket func
  int              ai_socktype;  // second arg to socket func
  int              ai_protocol;  // third arg to socket
  char            *ai_cannoname; // canonical hostname
  size_t           ai_addrlen;   // sizeof ai_addr struct
  struct sockaddr *ai_addr;      // ptr to socket address struct
  struct addrinfo *ai_next;      // ptr to next item in linked list.
};
*/ 
/**
 * AI_ADDRCONFIG: recommended if using connections, ask getaddrinfo to return IPv4 address only
 * AI_CANONNAME:  instructs getaddrinfo to point the ai_cannoame field in the first addrinfo struct
 * AI_NUMERICSERV: forces the service argument to be a port number
 * AI_PASSIVE: return socket addresses that can be used by servers as listening sockets
 */

/**
 * @brief converts string representa_tions of hostnames, host addresses, 
 * service names, and port numbers into socket address structure.
 * 
 * @param node domain names like <github.com>, do not include http(s).
 * @param service service name, can be nullptr.
 * @param hints specifies criteria for selecting the socket address 
 * structures returned in the list pointed to by res
 * @param[out] res point to start of output linked list
 */
void Getaddrinfo(const char *node, const char *service,
                 const struct addrinfo *hints,
                 struct addrinfo **res);
/**
 * @brief inverse of Getaddrinfo
 * 
 * @param sa sockaddr in addrinfo structs
 * @param salen ai_addrlen in addrinfo structs
 * @param[out] host output buffer for domain name 
 * @param host_len output buffer size(in bytes) for domain name
 * @param[out] service output buffer service name
 * @param servlen output buffer size(in bytes) for service name
 * @param flags NI_NUMERICHOST if numeric address string(eg. 127.0.0.1) is needed, 
 * NI_NUMERICSERV if port number is needed
 */
void Getnameinfo(const struct sockaddr *sa, socklen_t salen,
                 char *host, size_t host_len,
                 char *service, size_t servlen, int flags);
/**
 * @brief free the memory allocated when calling Getaddrinfo.
 * @return nothing 
 */
void freeaddrinfo(struct addrinfo *res);
/**< returns error message */
const char *gai_strerror(int errcode);

/**< zero out an addrinfo struct */
static inline void
init_addrinfo(struct addrinfo *ai) {
  memset((void *)ai, 0, sizeof(struct addrinfo));
}

/**
 * @brief helper subroutine that establish a connection with a server
 * @param hostname hostname of the targeted server
 * @param port port number of listening server
 * @returns descriptor if OK, terminate the program on error.
 */
sock_des_t open_clientfd(char *hostname, char *port);

/**
 * @param port port to listen to
 * @return a listening descriptor that is ready to receive connection request, 
 * -1 on error
 */
sock_des_t open_listenfd(char *port);

#endif // CSAPP_H
