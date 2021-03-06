#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define ROUNDS 100 // Number of ping-pong exchanges for statistical averaging

int main(int argc, char *argv[])
{
    int rank, size, init;
    double start_time, end_time, duration;
    int length = atoi(argv[1]);
    char *data = (char *)calloc(pow(2,length), sizeof(char)); // data chunk of size 2^length Bytes
    init = MPI_Init(NULL,NULL);
    
    if(init != MPI_SUCCESS)
        printf("\nMPI Code not initialized!");

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(size != 2)
    {
        printf("Exactly 2 processors need to be allocated for this task. Current allotment is %d\n", size);
        MPI_Abort(MPI_COMM_WORLD,1);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if(rank==0)
    {
        printf("\nStarting Measurement with data-size 2^%d Bytes", length);
        start_time = MPI_Wtime(); 
        for(int round=0; round<ROUNDS; round++)
        {   
            MPI_Send(data, pow(2,length), MPI_CHAR, 1, 10, MPI_COMM_WORLD);
            MPI_Recv(data, pow(2,length), MPI_CHAR, 1, 20, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        end_time = MPI_Wtime();

        duration = (end_time - start_time) / (2 * ROUNDS);
        double bandwidth = pow(2, length) / duration;
        printf("\nData size : 2^%d (B), Time (s) : %15.9f, Bandwidth (B/s) : %15.9f\n", length, duration, bandwidth);
    }
    else if(rank==1)
    {
        for(int round=0; round<ROUNDS; round++)
        {
            MPI_Recv(data, pow(2,length), MPI_CHAR, 0, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(data, pow(2,length), MPI_CHAR, 0, 20, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();

    free(data);

    return 0;
}