# This is solution to labs of CMU15-213
CMU15-213 is an excellent course on *Introduction to Computer System*.  
## Course info
[website](http://csapp.cs.cmu.edu/)
[labs](http://csapp.cs.cmu.edu/3e/labs.html)
Textbook *Computer System: A Programmer's Perspective*(a.k.a *csapp*)

# Progress
<table style="text-align:center">
  <tr>
    <td>Lab</td>
    <td> Progress </td>
    <td> Prerequisites</td>
    <td> Difficulty </td>
  </tr>
  <tr>
    <td>Lab 2: Bomb Lab</td>
    <td> 100% </td>
    <td> x86 asm, gdb </td>
    <td> 😇😇😇 </td>
  </tr>
  <tr>
    <td>Lab 7: Cache</td>
    <td> 70% </td>
    <td> C, cache basic </td>
    <td> 😇😇😇😇😇</td>
  </tr>
  <tr>
    <td>Lab 9: Shell</td>
    <td> 0% </td>
    <td> C, Process </td>
    <td> 😇😇😇😇😇</td>
  <tr>
    <td>Lab 10: Malloc</td>
    <td> 84% </td>
    <td> C, virtual memory</td>
    <td> 😇😇😇😇😇😇😇</td>
  </tr>
</table>

## Lab2: Bomb
Knowing basic [gdb](https://www.sourceware.org/gdb/) commands and proficiency of x86 is sufficient to pass up to `phase 5`.
`phase 6` requires tedious work(thus is rediculously difficult) though, you may choose to skip this phase😇.

## Lab5: Cache
In the first 40% of the lab, you'll design a cache simulator; then you'll design a matrix transpose algorithm to further your
understanding of how **priciples of locality** can lead to better performances.

## Lab7: Malloc
The worst choice I have ever made: using `simple segregated allocator`! This fails a lot of tests. Later I should
implement `segregated allocator` instead.

Currently I can only get up to `84` scores(100 total) for the record. This lab is hard and need constant work😇😇...
Pull requests welcome if you can provide a better solution.

# Why did I take this course
I want to further my understanding of Computer System and learn x86 assembly as well.