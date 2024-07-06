# Signal Cheatsheet 
This covers most contents in chapter 9 of csapp.

## getpid
```c
#include <sys/types.h>
#include <unistd.h>

pid_t getpid(void);
```
Get the current id of the process.

## fork
```c
#include <sys/types.h>
#include <unistd.h>

pid_t fork(void);
```
Create a child process. You can tell the parent process and child process apart in this way:
```c
pid_t pid;
if ((pid = fork()) == 0) {
  // child process
} else {
  // parent process
}
```

## execve
```c
#include <unistd.h>
int execve(const char *filename, const char **argv, const char **envp);
```
Returns -1 if executable not found.

## waitpid
Wait for the child to finish so as not to cause zombie process.
```c
#include <sys/type.h>
#include <sys/wait.h>
pid_t waitpid(pid_t pid, int *stat, int options);
```
Here's how to set `pid`.
<table>
  <tr> 
    <td>value</td>
    <td>meaning</td>
  </tr>
  <tr> 
    <td>>0</td>
    <td>wait for the child process with id=pid to finish</td>
  </tr>
  <tr> 
    <td>-1</td>
    <td>wait for any of its child process to finish</td>
  </tr>
</table>

Here's how to set `options`
<table>
  <tr>
    <td>0</td>
    <td>Default behavior: suspend execution util a child terminates</td>
  </tr>
  <tr>
    <td>WNOHANG</td>
    <td>return immediately if non terminated yet</td>
  </tr>
  <tr>
    <td>WUNTRACED</td>
    <td>suspend execution until one process become terminated or stopped </td>
  </tr>
  <tr>
    <td>WCONTINUED</td>
    <td>suspend execution until one process become terminated or received SIGCONT</td>
  </tr>
</table>

`stat` is one of output of this function. Set to NULL if not needed. Or you can use some 
macros to test the `stat`:
<table>
  <tr>
    <td>WIFEXITED(stat)</td>
    <td>true if child proc exited normally</td>
  </tr>
  <tr>
    <td>WEXITSTATUS(stat)</td>
    <td>exit status of child proc</td>
  </tr>
  <tr>
    <td>WIFSIGNALED(stat)</td>
    <td>exit because of unhandled signal</td>
  </tr>
  <tr>
    <td>WTEMSIG(stat)</td>
    <td>signals that caused termination</td>
  </tr>
  <tr>
    <td>WIFSTOPPED(stat)</td>
    <td>true if the proc is stopped at the time</td>
  </tr>
  <tr>
    <td>WSTOPSIG(stat)</td>
    <td>signals that stopped the proc</td>
  </tr>
  <tr>
    <td>WIFCONTINUED(stat)</td>
    <td>true if the proc received signal and continued</td>
  </tr>
</table>

## sleep, pause
```c
#include <unistd.h>
// return # seconds left
unsigned int sleep(unsigned int secs);
// hang until receive a signal
int pause(void);
```

## getpgrp
```c
#include <unistd.h>

// return process group id
pid_t gepgrp(void);
```

## setpgrg
Set the process group id of process `pid` to be `pgid`.
```c
#include <unistd.h>

// return 0 if success
int setpgid(pid_t pid, pid_t pgid);
```

## kill
Don't be confused by its name! It's used to send signal to other processes(including themselves).
```c
#include <sys/types.h>
#include <signal.h>

int kill(pid_t pid, int sig);
```

## signal
Used for receiving signals.
```c
#include <signal.h>
typedef void (* sighandler_t)(int);

sighandler_t signal(int signum, sighandler_t handler);
```
* If `handler` is `SIG_IGN`, then signal `signum` will be ignored.
* If `handler` is `SIG_DFL`, then will do default behavior.
* If `handler` is `SIG_ERR`, oops, fail.
* Else return user defined handler `handler`.

## masking
```c
#include <signal.h>
/** return 0 if succeed, -1 if abnormal */
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset); 
int sigemptyset(sigset_t *set); 
int sigfillset(sigset_t *set); 
int sigaddset(sigset_t *set, int signum); 
int sigdelset(sigset_t *set, int signum); 

/**
 * return 1 if signum is a member of set
 * return 0 if not, -1 if abnormal 
 */
int sigismember(const sigset_t *set, int signum); 
```
`how` can be one of the three:
<table>
  <tr>
    <td>SIG_BLOCK</td>
    <td>blocked = blocked | set</td>
  </tr>
  <tr>
    <td>SIG_UNBLOCK</td>
    <td>blocked = blocked & ~set</td>
  </tr>
  <tr>
    <td>SIG_SETMASK</td>
    <td>blocked = set</td>
  </tr>
</table>

## List of Signals
<table>
  <tr>
    <td>SIGINT</td>
    <td>Interrupt from keyboard</td>
  </tr>
  <tr>
    <td>SIGKILL</td>
    <td>Kill program</td>
  </tr>
  <tr>
    <td>SIGSEGV</td>
    <td>Invalid memory access</td>
  </tr>
</table>

# IO Cheatsheet
This covers most contents in chapter 10 of csapp.

## File Descriptor
```c
#include <unistd.h>

/** file descriptor for stdin, stdout, stderr */
#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
```

## Open, Close File
```c
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/** open file, return file descriptor, -1 if failure */
int open(char *filename, int flags, mode_t mode);
/** close file, return 0 if success, -1 o.w. */
int close(int fd);
```
`flags` can be some of the following:
<table>
  <tr>
    <td>O_RDONLY</td>
    <td>Read only</td>
  </tr>
  <tr>
    <td>O_WRONLY</td>
    <td>Write only</td>
  </tr>
  <tr>
    <td>O_RDWR</td>
    <td>Read or write</td>
  </tr>
  <tr>
    <td>O_CREAT</td>
    <td>create file if not exists</td>
  </tr>
  <tr>
    <td>O_TRUNC</td>
    <td>truncate the file if exists</td>
  </tr>
  <tr>
    <td>O_APPEND</td>
    <td>Append to the back of the file</td>
  </tr>
</table>

`mode` can be simply set to 0.

## Read, Write File
```c
#include <unistd.h>

/** return number of bytes read, 0 if EOF, -1 if failure. */
ssize_t read(int fd, void *buf, size_t n);
/** return number of bytes write, 0 if EOF, -1 if failure. */
ssize_t write(int fd, const void *buf, size_t n);
```

## Read Metadata of Files
```c
#include <unistd.h>
#include <sys/stat.h>

/**< returns -1 on error */
int stat(const char *filename, struct stat *st);
int fstat(int fd, struct stat *st);
```

The `struct stat` looks like: 
```c
/**< metadata returned by the stat and fstat */
struct stat {
  dev_t         st_dev;      // device
  ino_t         st_ino;      // inode
  mode_t        st_mode;     // protection, file type
  nlink_t       st_nlink;    // number of hard links
  uid_t         st_uid;      // user id of owner
  gid_t         st_gid;      // group id of owner
  dev_t         st_rdev;     // device type(if inode device)
  off_t         st_size;     // total size in bytes
  unsigned long st_blksize;  // Block size for fs IO
  unsigned long st_blocks;   // Number of blocks allocated
  time_t        st_atime;    // time of last access
  time_t        st_mtime;    // time of last modification
  time_t        st_ctime;    // time of last change
};
```
We're interested in `st_mode` and `st_size` when building our web server.

## Read Directory
User programs can open, read, close a directory:
```c
#include <sys/types.h>
#include <dirent.h>
/**< returns NULL on error */
DIR *opendir(const char *fname);
/**< returns -1 on error */
int closedir(DIR *dir);
/**< returns NULL if no more files or error */
struct dirent *readdir(DIR *dirp);
```

And `struct dirent` is like:
```c
struct dirent {
  ino_t d_ino;        // inode number
  char  d_name[256];  // file name
}
```

## IO Redirection
```c
/**< return -1 on error */
int dup2(int oldfd, int newfd);
```

From the textbook, 
> The dup2 function copies descriptor table entry oldfd to descriptor table entfy 
> newfd, overwriting the previous contents of descriptor table entrynewfd. If newfd 
> was already open, then dup2 closes newfd before it copies oldfd. 

# Networking Cheatsheet
This covers most contents in chapter 11 of csapp.

## IP Address
Ip address is a 32-bit unsigned integer:
```c
/**< here "in" stands for internet */
struct in_addr {
  uint32_t s_addr;
};
```

`htonl` converts 32-bit host byte order to network byte order, and `ntohl` do the opposite:
```c
#include <arpa/inet.h>

uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);

uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);
```

## Socket Interface
```c
/**< IP socket address structure */
struct sockaddr_in {
  uint16_t       sin_family;  // protocol family
  uint16_t       sin_port;    // port number in network byte order
  struct in_addr sin_addr;    // IP address in network byte order
  unsigned char  sin_zero[8]; // Pad to sizeof(struct sockaddr)
};

/**< Generic socket address structure */
struct sockaddr {
  uint16_t sa_family;    // protocol family
  char     sa_data[14];  // Address data
};
```

Some useful methods:
```c
#include <sys/types.h>
#include <sys/socket.h>

/**< create a socket descriptor, return -1 on err */
int socket(int domain, int type, int protocol);
```
`domain` is `AF_INET` if we're using 32-bit IP address;  `type` may be `SOCK_STREAM`, `protocol` may be 0.
Note that the descriptor returned by this method is half-opened and cannot be used for reading/writing.

```c
/**< client method, return -1 on error. */
int connect(int clientfd, const struct sockaddr *addr, socklen_t addrlen);
```
Client side try to make a connection to the server.

> Bind, listen and accept method is used in server side.

```c
/**< returns -1 on error */
int bind(int sockfd, struct sockaddr *addr, socklen_t addrlen);
```
Bind method asks the kernel to associate the server socket address with the socket descriptor `sockfd`.
The `addrlen` is `sizeof(sockaddr_in)`.

```c
/**< returns -1 on error */
int listen(int sockfd, int backlog);
```
The listen function converts `sockfd` from air active socket to a listening socket 
that can accept connection requests from clients. We typically set `backlog` to a large value
(e.g. 1024).

```c
/**< returns active connected descriptor, -1 on error */
int accept(int listenfd, struct sockaddr *addr, int *addrlen);
```
The accept function waits for a connection request from a client to arrive on 
the listening descriptor listened, then fills in the client's socket address in `addr`.
and returns a connected discriptor that can be used to communicate with the 
client using Unix I/O functions.

> Host and service conversion

```c
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

/**< addr info used by getaddrinfo */
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
/**
 * AI_ADDRCONFIG: recommended if using connections, ask getaddrinfo to return IPv4 address only
 * AI_CANONNAME:  instructs getaddrinfo to point the ai_cannoame field in the first addrinfo struct
 * AI_NUMERICSERV: forces the service argument to be a port number
 * AI_PASSIVE: return socket addresses that can be used by servers as listening sockets
 */
```
`ai_flags` have two options: `AF_INET` or `AF_INET6`.

```c
/**
 * @brief converts string representa_tions of hostnames, host addresses, 
 * service names, and port numbers into socket address structure.
 * 
 * @param node domain names like <github.com>, do not include http(s).
 * @param service service name, can be nullptr.
 * @param hints specifies criteria for selecting the socket address 
 * structures returned in the list pointed to by res
 * @param[out] res point to start of output linked list
 * @return 0 if success
 */
int getaddrinfo(const char *node, const char *service,
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
 * @return 0 if success
 */
int getnameinfo(const struct sockaddr *sa, socklen_t salen,
                char *host, size_t host_len,
                char *service, size_t servlen, int flags);
/**< returns nothing */
void freeaddrinfo(struct addrinfo *res);
/**< returns error message */
const char *gai_strerror(int errcode);
```

> Don't get confused by these functions! They are powerful at the very least.

# Thread Cheatsheet

## Create a Thread
`Pthread` is the thread library to use.
```c
#include <pthread.h>

/**
 * @param[out] thread thread id of created thread
 * @param attr set to NULL for default behavior
 * @param fn function to execute
 * @param arg argument to the function
 * @return 0 on success 
 */
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(* fn) (void *), void *arg);
```

`pthread_self` return the id of a thread:
```c
pthread_t pthread_self();
```

`pthread_join` suspend execution of calling thread until the targeted thread terminates. 
```c
/**< return 0 on success */
int pthread_join(pthread_t tid, void **thread_return);
```

`pthread_detach` makes memory leak impossible even if you do not explicitly free the memory 
used by a thread.
```c
int pthread_detach(pthread_t tid);
```

Locks solves a lot of concurrency problems, you should know these:
```c
/**< return 0 if OK */
/**< initialize the mutex, set attr to NULL for default behavior */
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
/**< acquire the mutex */
int pthread_mutex_lock(pthread_mutex_t *mutex);
/**< release the mutex */
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```
