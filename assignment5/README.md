#Compiling the Program
##
1. make clean
2. make
3. mpiexec -np 2 ./pingpong "power of two (1, 2,...20)"
    e.g : mpiexec -np 2 ./pingpong 8