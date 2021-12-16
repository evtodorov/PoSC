Sample input
### mpirun -n 16 ./heat test.dat 4 4

Sample output
```
rank= 2 coords= 0 2 neighbours(up,down,left,right)= 14 6 1 3
rank= 0 coords= 0 0 neighbours(up,down,left,right)= 12 4 3 1
rank= 1 coords= 0 1 neighbours(up,down,left,right)= 13 5 0 2
rank= 3 coords= 0 3 neighbours(up,down,left,right)= 15 7 2 0
rank= 4 coords= 1 0 neighbours(up,down,left,right)= 0 8 7 5
rank= 5 coords= 1 1 neighbours(up,down,left,right)= 1 9 4 6
rank= 6 coords= 1 2 neighbours(up,down,left,right)= 2 10 5 7
rank= 8 coords= 2 0 neighbours(up,down,left,right)= 4 12 11 9
rank= 9 coords= 2 1 neighbours(up,down,left,right)= 5 13 8 10
rank= 10 coords= 2 2 neighbours(up,down,left,right)= 6 14 9 11
rank= 7 coords= 1 3 neighbours(up,down,left,right)= 3 11 6 4
rank= 11 coords= 2 3 neighbours(up,down,left,right)= 7 15 10 8
rank= 12 coords= 3 0 neighbours(up,down,left,right)= 8 0 15 13
rank= 13 coords= 3 1 neighbours(up,down,left,right)= 9 1 12 14
rank= 14 coords= 3 2 neighbours(up,down,left,right)= 10 2 13 15
rank= 15 coords= 3 3 neighbours(up,down,left,right)= 11 3 14 12
```

Sample input
### mpirun -n 4 ./heat test.dat 1 4

Sample output
```
rank= 0 coords= 0 0 neighbours(up,down,left,right)= -1 -1 3 1
rank= 1 coords= 0 1 neighbours(up,down,left,right)= -1 -1 0 2
rank= 2 coords= 0 2 neighbours(up,down,left,right)= -1 -1 1 3
rank= 3 coords= 0 3 neighbours(up,down,left,right)= -1 -1 2 0
```

Sample input
### mpirun -n 9 ./heat test.dat 3 3

Sample output
```
rank= 0 coords= 0 0 neighbours(up,down,left,right)= 6 3 2 1
rank= 1 coords= 0 1 neighbours(up,down,left,right)= 7 4 0 2
rank= 2 coords= 0 2 neighbours(up,down,left,right)= 8 5 1 0
rank= 3 coords= 1 0 neighbours(up,down,left,right)= 0 6 5 4
rank= 4 coords= 1 1 neighbours(up,down,left,right)= 1 7 3 5
rank= 5 coords= 1 2 neighbours(up,down,left,right)= 2 8 4 3
rank= 6 coords= 2 0 neighbours(up,down,left,right)= 3 0 8 7
rank= 7 coords= 2 1 neighbours(up,down,left,right)= 4 1 6 8
rank= 8 coords= 2 2 neighbours(up,down,left,right)= 5 2 7 6
```
