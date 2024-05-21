# CS:APP Shell Lab

Files:

<table style="text-align:center;">
<tr>
<td>Makefile	</td>
<td> Compiles your shell program and runs the tests</td>
</tr>

<tr>
 <td>README</td> <td>this file </td>
</tr>
<tr>
<td>tsh.c</td>		
<td> The shell program that you will write and hand in</td>
</tr>
<td> tshref</td>	<td> The reference shell binary</td>
</table>

## The remaining files are used to test your shell
sdriver.pl	# The trace-driven shell driver
trace*.txt	# The 15 trace files that control the shell driver
tshref.out 	# Example output of the reference shell on all 15 traces

## Little C programs that are called by the trace files
myspin.c	# Takes argument <n> and spins for <n> seconds
mysplit.c	# Forks a child that spins for <n> seconds
mystop.c        # Spins for <n> seconds and sends SIGTSTP to itself
myint.c         # Spins for <n> seconds and sends SIGINT to itself

# Cheatsheet 
This covers some(a lot of) useful system calls/ functions to complete this lab.
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
int execve(const char *filename, const char **argv,
           const char *envp);
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