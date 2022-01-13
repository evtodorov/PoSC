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
	double final_residual;

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
	periods[0] = 0; //false
	periods[1] = 0; //false
	// if(dims[0] == 1)
	// 	periods[0] = 0;
	// if(dims[1] == 1)
	// 	periods[1] = 0;
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
	}

	time = (double *) calloc(sizeof(double), (int) (param.max_res - param.initial_res + param.res_step_size) / param.res_step_size);

	int exp_number = 0;

	// Create Virtual topology
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &comm_2d);
	MPI_Cart_coords(comm_2d, rank, 2, coord);
	MPI_Cart_shift(comm_2d, 0, 1, &up, &down);
	MPI_Cart_shift(comm_2d, 1, 1, &left, &right);
	printf("rank= %d coords= %d %d neighbours(up,down,left,right)= %d %d %d %d\n", rank, coord[0], coord[1], up, down, left, right); 

	MPI_Barrier(comm_2d);

	for (param.act_res = param.initial_res; param.act_res <= param.max_res; param.act_res = param.act_res + param.res_step_size) {
		np = param.act_res + 2;
		int send_count = np * np;

		// configure grid NB doesn't do any memory allocation; usage TBD
		gridparam_t gridparam;
		gridparam.grid_num_rows = dims[0];
		gridparam.grid_num_cols = dims[1];
		gridparam.grid_row = coord[0];
		gridparam.grid_col = coord[1];
		configure_grid(&param, &gridparam);

		int col_start = gridparam.store_col_start;
		int col_end = gridparam.store_col_end;
		int row_start = gridparam.store_row_start;
		int row_end = gridparam.store_row_end;
		const int np_cols = col_end - col_start + 1;
		const int np_rows = row_end - row_start + 1;

		printf("\nNum-X: %d ; Num-Y: %d ; Resolution: %d ; Rank: %d ; Rank-X: %d ; Rank-Y: %d ; Grid_Rows: %d; Grid_Cols: %d; Grid_Row: %d; Grid_Col: %d; Grid  Row_start: %d; Row_end: %d; Col_start: %d; Col_end: %d, Total_cols: %d, Total_rows: %d\n", 
				dims[0], dims[1],
				param.act_res, rank, coord[0], coord[1], gridparam.grid_num_rows, gridparam.grid_num_rows, gridparam.grid_row, gridparam.grid_col,
				row_start, row_end,
				col_start, col_end, np_cols, np_rows);

		MPI_Barrier(comm_2d);

		if (!initialize(&param, &gridparam)) {
			fprintf(stderr, "Error in Jacobi initialization.\n\n");
			usage(argv[0]);
		}

		for (i = 0; i < np_rows; i++) {
			for (j = 0; j < np_cols; j++) {
				param.uhelp[i * (np_cols) + j] = param.u[i * (np_cols) + j];
			}
		}

		for(int i = 0; i<size;i++){
			if(rank == i){
				printf("Rank : %d\n", rank);
				for (int i=0; i < np_rows; i++){
					for (int j=0; j < np_cols; j++){
						printf("%f ", param.u[i*np_cols+j]);
					}
					printf("\n");
				}
			}
			MPI_Barrier(comm_2d);
		}

		MPI_Barrier(MPI_COMM_WORLD);

		// starting time
		time[exp_number] = wtime();
		residual = 999999999;
		np = param.act_res + 2;

		//initialise arrays to store strides
		double *oben, *unten, *links, *rechts ,*oben_help, *unten_help, *links_help, *rechts_help;
		oben = (double*)malloc( sizeof(double)* np_cols);
		unten = (double*)malloc( sizeof(double)* np_cols);
		links = (double*)malloc( sizeof(double)* np_rows);
		rechts = (double*)malloc( sizeof(double)* np_rows);
		oben_help = (double*)malloc( sizeof(double)* np_cols);
		unten_help = (double*)malloc( sizeof(double)* np_cols);
		links_help = (double*)malloc( sizeof(double)* np_rows);
		rechts_help = (double*)malloc( sizeof(double)* np_rows);

		for (iter = 0; iter < param.maxiter; iter++) {
			residual = relax_jacobi(&(param.u), &(param.uhelp), np_rows, np_cols);

			for(int i = 0; i < np_cols; i++) {
				oben[i] = param.u[i];
			}
			
			for(int i = 0; i < np_cols; i++) {
				unten[i] = param.u[(np_cols - 1)* np_cols + i];
			}

			for(int i = 0; i < np_rows; i++) {
				links[i] = param.u[i*np_cols];
				rechts[i] = param.u[i*np_cols+(np_cols-1)];
			}

			if (dims[0] > 1){
				// send left, recv right
				// send right, recv left
				if (left==-1){
					MPI_Recv(rechts_help, np_rows, MPI_DOUBLE, right, 10, MPI_COMM_WORLD, &s);
					MPI_Send(rechts, np_rows, MPI_DOUBLE, right, 20, MPI_COMM_WORLD);
				}
				else if (right==-1){
					MPI_Send(links, np_rows, MPI_DOUBLE, left, 10, MPI_COMM_WORLD);
					MPI_Recv(links_help, np_rows, MPI_DOUBLE, left, 20, MPI_COMM_WORLD, &s);
				}
				else if (left!=-1){				
					MPI_Recv(rechts_help, np_rows, MPI_DOUBLE, right, 10, MPI_COMM_WORLD, &s);
					MPI_Send(rechts, np_rows, MPI_DOUBLE, right, 20, MPI_COMM_WORLD);
				}
				else if (right!=-1){
					MPI_Send(links, np_rows, MPI_DOUBLE, left, 10, MPI_COMM_WORLD);
					MPI_Recv(links_help, np_rows, MPI_DOUBLE, left, 20, MPI_COMM_WORLD, &s);
				}
			}

			if (dims[1]>1){
				// send up, recv down
				// send down, recv up
				if (up==-1){
					MPI_Recv(unten_help, np_cols, MPI_DOUBLE, down, 30, MPI_COMM_WORLD, &s);
					MPI_Send(unten, np_cols, MPI_DOUBLE, down, 40, MPI_COMM_WORLD);
				}
				else if (down==-1){
					MPI_Send(oben, np_cols, MPI_DOUBLE, up, 30, MPI_COMM_WORLD);
					MPI_Recv(oben_help, np_cols, MPI_DOUBLE, up, 40, MPI_COMM_WORLD, &s);
				}
				else if (up!=-1){
					MPI_Recv(unten_help, np_cols, MPI_DOUBLE, down, 30, MPI_COMM_WORLD, &s);
					MPI_Send(unten, np_cols, MPI_DOUBLE, down, 40, MPI_COMM_WORLD);
				}
				else if (down!=-1){
					MPI_Send(oben, np_cols, MPI_DOUBLE, up, 30, MPI_COMM_WORLD);
					MPI_Recv(oben_help, np_cols, MPI_DOUBLE, up, 40, MPI_COMM_WORLD, &s);
				}
			}
			
			MPI_Reduce(&residual, &final_residual, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

			for(int i = 0; i < np_cols; i++) {
				param.u[i] = oben_help[i];
			}
			
			for(int i = 0; i < np_cols; i++) {
				param.u[(np_cols - 1)* np_cols + i] = unten_help[i];
			}

			for(int i = 0; i < np_rows; i++) {
				param.u[i*np_cols] = links_help[i];
				param.u[i*np_cols+(np_cols-1)] = rechts_help[i];
			}
		}

		free(oben);
		free(unten);
		free(links);
		free(rechts);
		free(oben_help);
		free(unten_help);
		free(links_help);
		free(rechts_help);

		time[exp_number] = wtime() - time[exp_number];

		printf("\n\nResolution: %u\n", param.act_res);
		printf("===================\n");
		printf("Execution time: %f\n", time[exp_number]);
		printf("Residual: %f\n\n", residual);
		if(rank == 0)
			printf("Final Residual: %f\n\n", final_residual);

		printf("megaflops:  %.1lf\n", (double) param.maxiter * (np - 2) * (np - 2) * 7 / time[exp_number] / 1000000);
		printf("  flop instructions (M):  %.3lf\n", (double) param.maxiter * (np - 2) * (np - 2) * 7 / 1000000);
		
		exp_number++;
	}

	param.act_res = param.act_res - param.res_step_size;

	// coarsen(param.u, param.act_res + 2, param.act_res + 2, param.uvis, param.visres + 2, param.visres + 2);

	// if(rank == 0)
	// 	write_image(resfile, param.uvis, param.visres + 2, param.visres + 2);

	finalize(&param);

	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Finalize();

	return 0;
}
