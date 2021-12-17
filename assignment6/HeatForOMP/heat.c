#include <stdio.h>
#include <stdlib.h>
#include "input.h"
#include "heat.h"
#include "timing.h"

#include <mpi.h>

double* time;

void usage(char *s) {
	fprintf(stderr, "Usage: %s <input file> [result file]\n\n", s);
}

int main(int argc, char *argv[]) {
	int i, j, k, ret;
	FILE *infile, *resfile;
	char *resfilename;
	int np, iter, chkflag;
	int dims[2], periods[2], coord[2], reorder;
	int up, down, left, right;

	
	// algorithmic parameters
	algoparam_t param;

	// timing

	double residual;

	// set the visualization resolution
	param.visres = 100;

	int rank, size;
	MPI_Status s;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm comm_2d;


	// check arguments
	if (argc < 4) {
		usage(argv[0]);
		return 1;
	}

	// check input file
	if (!(infile = fopen(argv[1], "r"))) {
		fprintf(stderr, "\nError: Cannot open \"%s\" for reading.\n\n", argv[1]);

		usage(argv[0]);
		return 1;
	}

	// Dimensions for virtual topology
	dims[0] = atoi(argv[2]); // X-dimension
	dims[1] = atoi(argv[3]); // Y-dimension
	periods[0] = 1;
	periods[1] = 1;
	if(dims[0] == 1)
		periods[0] = 0;
	if(dims[1] == 1)
		periods[1] = 0;
	reorder = 0;

    if(size != dims[0] * dims[1])
    {
        printf("\nThe product of dimensions %d is not equal to number of requested processes %d\n", dims[0]*dims[1], size);
        MPI_Abort(MPI_COMM_WORLD,1);
    }

	if (rank == 0)
		printf("\n%d processes distributed in X-direction\n %d ; processes distributed in Y-direction\n", dims[0], dims[1]);


	// check result file
	resfilename = (argc >= 3) ? argv[2] : "heat.ppm";

	if (!(resfile = fopen(resfilename, "w"))) {
		fprintf(stderr, "\nError: Cannot open \"%s\" for writing.\n\n", resfilename);

		usage(argv[0]);
		return 1;
	}

	// check input
	if (!read_input(infile, &param)) {
		fprintf(stderr, "\nError: Error parsing input file.\n\n");

		usage(argv[0]);
		return 1;
	}

	if(rank == 0)
	{
		print_params(&param);
		printf("\n\nResolution: %u\n", param.act_res);
			printf("===================\n");
	}

	time = (double *) calloc(sizeof(double), (int) (param.max_res - param.initial_res + param.res_step_size) / param.res_step_size);

	int exp_number = 0;

	// Create Virtual topology
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &comm_2d);
	MPI_Cart_coords(comm_2d, rank, 2, coord);
	MPI_Cart_shift(comm_2d, 0, 1, &up, &down);
	MPI_Cart_shift(comm_2d, 1, 1, &left, &right);

	for (param.act_res = param.initial_res; param.act_res <= param.max_res; param.act_res = param.act_res + param.res_step_size) {
		np = param.act_res + 2;
		int send_count = np * np;

		
		if (!initialize(&param)) {
			fprintf(stderr, "Error in Jacobi initialization.\n\n");
			usage(argv[0]);
		}

		// configure grid NB doesn't do any memory allocation; usage TBD
		gridparam_t gridparam;
		gridparam.grid_num_rows = dims[0];
		gridparam.grid_num_cols = dims[1];
		gridparam.grid_row = coord[0];
		gridparam.grid_col = coord[1];
		configure_grid(&param, &gridparam);

		if (rank==0)
		{
			printf("\n\nResolution: %u\n", param.act_res);
			printf("===================\n");
		}

		printf("\nRank: %d ; Coord: %d %d ; Rows: %d; Cols: %d", rank, coord[0], coord[1], 
				gridparam.store_row_end - gridparam.store_row_start, 
				gridparam.store_col_end - gridparam.store_col_start);

		for (i = 0; i < param.act_res + 2; i++) {
			for (j = 0; j < param.act_res + 2; j++) {
				param.uhelp[i * (param.act_res + 2) + j] = param.u[i * (param.act_res + 2) + j];
			}
		}

		// starting time
		time[exp_number] = wtime();
		residual = 999999999;
		np = param.act_res + 2;

		for (iter = 0; iter < param.maxiter; iter++) {
			residual = relax_jacobi(&(param.u), &(param.uhelp), np, np);
		}

		time[exp_number] = wtime() - time[exp_number];

		// if (rank==0)
		// {
		// 	printf("\n\nResolution: %u\n", param.act_res);
		// 	printf("===================\n");
		// 	printf("Execution time: %f\n", time[exp_number]);
		// 	printf("Residual: %f\n\n", residual);

		// 	printf("megaflops:  %.1lf\n", (double) param.maxiter * (np - 2) * (np - 2) * 7 / time[exp_number] / 1000000);
		// 	printf("  flop instructions (M):  %.3lf\n", (double) param.maxiter * (np - 2) * (np - 2) * 7 / 1000000);

		// }
		
		exp_number++;
	}

	param.act_res = param.act_res - param.res_step_size;

	coarsen(param.u, param.act_res + 2, param.act_res + 2, param.uvis, param.visres + 2, param.visres + 2);

	write_image(resfile, param.uvis, param.visres + 2, param.visres + 2);

	finalize(&param);

	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Finalize();

	return 0;
}
