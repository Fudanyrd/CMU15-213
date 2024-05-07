# Lab 5: Cache

## Cache Simulation
There isn't much trap here(although coding work is quite heavy😇). Test results:
```
root@yrd:/mnt/c/Users/86182/Desktop/CMU15-13/cache# ./test-csim
                        Your simulator     Reference simulator
Points (s,E,b)    Hits  Misses  Evicts    Hits  Misses  Evicts
     3 (1,1,1)       9       8       6       9       8       6  traces/yi2.trace
     3 (4,2,4)       4       5       2       4       5       2  traces/yi.trace
     3 (2,1,4)       2       3       1       2       3       1  traces/dave.trace
     3 (2,1,3)     167      71      67     167      71      67  traces/trans.trace
     3 (2,2,3)     201      37      29     201      37      29  traces/trans.trace
     3 (2,4,3)     212      26      10     212      26      10  traces/trans.trace
     3 (5,1,5)     231       7       0     231       7       0  traces/trans.trace
     6 (5,1,5)  265189   21775   21743  265189   21775   21743  traces/long.trace
    27

TEST_CSIM_RESULTS=27
```
Memory errors(including leaks): None
```
==8127== 
==8127== HEAP SUMMARY:
==8127==     in use at exit: 0 bytes in 0 blocks
==8127==   total heap usage: 38 allocs, 38 frees, 4,016 bytes allocated
==8127== 
==8127== All heap blocks were freed -- no leaks are possible
```
Note that `fopen` must be followed by `fclose`😇, else there will be memory leakages!

## Transpose Matrix
<table>
  <tr>
    <td> Test Case </td>
    <td> Misses</td>
    <td> Score </td>
    <td> Max Score </td>
  </tr>
  <tr>
    <td> 32x32 </td>
    <td> 260 </td>
    <td style="color:green"> 8 </td>
    <td> 8 </td>
  </tr>
  <tr>
    <td> 64x64 </td>
    <td> 1604 </td>
    <td style="color:red"> 4.5 </td>
    <td> 8 </td>
  </tr>
  <tr>
    <td> 67x61 </td>
    <td> 2314</td>
    <td style="color:red"> 6.9 </td>
    <td> 10 </td>
  </tr>
</table>