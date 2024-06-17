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
