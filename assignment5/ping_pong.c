#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define ROUNDS 100

int main(int argc, char *argv[])
{
    int rank, size, init;
    double start_time, end_time, duration;
    int length = atoi(argv[1]);
    char data[length];
    init = MPI_Init(NULL,NULL);
    
    if(init != MPI_SUCCESS)
        printf("\nMPI Code not initialized!");

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    MPI_Barrier(MPI_COMM_WORLD);

    if(rank==0)
    {
        printf("\nData Size is of power : %d", length);
        printf("\nStarting Meaurement with data-size 2^%d", length);
        start_time = MPI_Wtime(); 
        for(int round=0; round<ROUNDS; round++)
        {   
            
            MPI_Send(&data[0], length, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&data[0], length, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        end_time = MPI_Wtime();
        duration = (end_time - start_time) / ROUNDS;
        printf("\nData size: 2^%d, Time : %lf \n", length, duration);            
    }
    else
    {
        for(int round=0; round<ROUNDS; round++)
        {
            MPI_Recv(&data[0], length, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&data[0], length, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        }

    }
    MPI_Finalize();
}