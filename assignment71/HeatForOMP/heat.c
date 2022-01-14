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

	int nprows, npcols;
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
	periods[0] = 0;
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
	}

	time = (double *) calloc(sizeof(double), (int) (param.max_res - param.initial_res + param.res_step_size) / param.res_step_size);

	int exp_number = 0;

	// Create Virtual topology
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &comm_2d);
	MPI_Cart_coords(comm_2d, rank, 2, coord);
	MPI_Cart_shift(comm_2d, 0, 1, &up, &down);
	MPI_Cart_shift(comm_2d, 1, 1, &left, &right);
	printf("rank= %d coords= %d %d neighbours(up,down,left,right)= %d %d %d %d\n", rank, coord[0], coord[1], up, down, left, right); 

	for (param.act_res = param.initial_res; param.act_res <= param.max_res; param.act_res = param.act_res + param.res_step_size) {

		// configure grid NB doesn't do any memory allocation; usage TBD
		gridparam_t gridparam;
		gridparam.grid_num_rows = dims[0];
		gridparam.grid_num_cols = dims[1];
		gridparam.grid_row = coord[0];
		gridparam.grid_col = coord[1];
		configure_grid(&param, &gridparam);

		
		if (!initialize(&param, &gridparam)) {
			fprintf(stderr, "Error in Jacobi initialization.\n\n");
			usage(argv[0]);
		}

		printf("\nNum-X: %d ; Num-Y: %d ; Resolution: %d ; Rank: %d ; Rank-X: %d ; Rank-Y: %d ; Rowstart: %d; Rowend: %d; Rows: %d; Colstart: %d; Colend: %d; Cols: %d;", 
				dims[0], dims[1], 
				param.act_res, rank, coord[0], coord[1], 
				gridparam.store_row_start, gridparam.store_row_end,
				gridparam.store_row_end - gridparam.store_row_start,
			    gridparam.store_col_start, gridparam.store_col_end,	       
				gridparam.store_col_end - gridparam.store_col_start);

		// local number of points
		nprows = gridparam.store_row_end - gridparam.store_row_start + 1;
		npcols = gridparam.store_col_end - gridparam.store_col_start + 1;

		for (i = 0; i < nprows; i++) {
			for (j = 0; j <npcols; j++) {
				param.uhelp[i * npcols + j] = param.u[i * npcols + j];
			}
		}

		// starting time
		time[exp_number] = wtime();
		residual = 999999999;
		np = param.act_res + 2;

		MPI_Status status;
		double *firstcolumn, *secondcolumn, *lastcolumn, *secondtolastcolumn;
		firstcolumn = (double*)malloc( sizeof(double)* nprows );
		lastcolumn = (double*)malloc( sizeof(double)* nprows );
		secondcolumn = (double*)malloc( sizeof(double)* nprows );
		secondtolastcolumn = (double*)malloc( sizeof(double)* nprows );
		double tot_residual = 0;
		for (iter = 0; iter < param.maxiter; iter++) {
			/*MPI_Barrier(comm_2d);
			for (int r=0; r<size; r++){
				if (rank==r){
						printf("\n-------------\nRank:%u\n", rank);
						print_array(param.u, npcols, nprows);
						printf("\n");
				}
			MPI_Barrier(comm_2d);
			}
			*/
			residual = relax_jacobi(&(param.u), &(param.uhelp), npcols, nprows);

			// Communicate
			if (dims[0] > 1){
				for (int i=0; i < nprows; i++){
					secondcolumn[i] = param.u[i*npcols+1]; 
					secondtolastcolumn[i] = param.u[(i+1)*npcols-2];
				}
				// send left, recv right
				// send right, recv left
				if (left==-1){
					MPI_Recv(lastcolumn, nprows, MPI_DOUBLE, right, 10,
							comm_2d, &status);
					MPI_Send(secondtolastcolumn, nprows, MPI_DOUBLE, right, 20,
							comm_2d);
					for (int i=0; i < nprows; i++){
						param.u[(i+1)*npcols-1] = lastcolumn[i]; 
					} 
				}
				else if (right==-1){
					MPI_Send(secondcolumn, nprows, MPI_DOUBLE, left, 10,
							comm_2d);
					MPI_Recv(firstcolumn, nprows, MPI_DOUBLE, left, 20,
							comm_2d, &status);
					for (int i=0; i < nprows; i++){
						param.u[i*npcols] = firstcolumn[i];
					} 
				}
				else {				
					MPI_Sendrecv(secondcolumn, nprows, MPI_DOUBLE, left, 10,
								lastcolumn, nprows, MPI_DOUBLE, right, 10, 
								comm_2d, &status);
					MPI_Sendrecv(secondtolastcolumn, nprows, MPI_DOUBLE, right, 20,
								firstcolumn, nprows, MPI_DOUBLE, left, 20, 
								comm_2d, &status);
					for (int i=0; i < nprows; i++){
						param.u[i*npcols] = firstcolumn[i];
						param.u[(i+1)*npcols-1] = lastcolumn[i]; 
					} 
				}

			}

			if (dims[1]>1){
				// send up, recv down
				// send down, recv up
				if (up==-1){
					MPI_Recv((param.u+(nprows-1)*npcols), npcols, MPI_DOUBLE, down, 30, 
							comm_2d, &status);
					MPI_Send((param.u+(nprows-2)*npcols), npcols, MPI_DOUBLE, down, 40,
							comm_2d);
				}
				else if (down==-1){
					MPI_Send((param.u+npcols), npcols, MPI_DOUBLE, up, 30,
							comm_2d);
					MPI_Recv((param.u), npcols, MPI_DOUBLE, up, 40, 
								comm_2d, &status);
				}
				else {
					MPI_Sendrecv((param.u+npcols), npcols, MPI_DOUBLE, up, 30,
								(param.u+(nprows-1)*npcols), npcols, MPI_DOUBLE, down, 30, 
								comm_2d, &status);
					MPI_Sendrecv((param.u+(nprows-2)*npcols), npcols, MPI_DOUBLE, down, 40,
								(param.u), npcols, MPI_DOUBLE, up, 40, 
								comm_2d, &status);
				}
			}
		}
		free(firstcolumn); free(lastcolumn); free(secondcolumn); free(secondtolastcolumn);

		time[exp_number] = wtime() - time[exp_number];
		MPI_Reduce(&residual, &tot_residual, 1, MPI_DOUBLE, MPI_SUM, 0, comm_2d);
		if (rank==0)
		{
			printf("\n\nResolution: %u\n", param.act_res);
			printf("===================\n");
			printf("Execution time: %f\n", time[exp_number]);
			printf("Residual: %f\n\n", tot_residual);

			printf("megaflops:  %.1lf\n", (double) param.maxiter * (np - 2) * (np - 2) * 7 / time[exp_number] / 1000000);
			printf("  flop instructions (M):  %.3lf\n", (double) param.maxiter * (np - 2) * (np - 2) * 7 / 1000000);

		}
		
		exp_number++;
	}

	param.act_res = param.act_res - param.res_step_size;

	gridparam_t gridparam;
	gridparam.grid_num_rows = dims[0];
	gridparam.grid_num_cols = dims[1];
	gridparam.grid_row = coord[0];
	gridparam.grid_col = coord[1];
	configure_grid(&param, &gridparam);
	int vprows = gridparam.vis_row_end - gridparam.vis_row_start;
	int vpcols = gridparam.vis_col_end - gridparam.vis_col_start;

	coarsen(param.u, npcols, nprows, param.uvis, vpcols, vprows);

	// collect
	if (rank==0){
		double *uvisfinal, *uvistemp;
		uvisfinal  = (double*)calloc( sizeof(double), (param.visres + 2)*(param.visres + 2) );
		uvistemp = (double*)calloc( sizeof(double), (param.visres + 2)*(param.visres + 2) );
		MPI_Status status;
		for (int rx=0; rx < dims[0]; rx++){
			for (int ry=0; ry < dims[1]; ry++){
				gridparam.grid_row = rx;
				gridparam.grid_col = ry;
				configure_grid(&param, &gridparam);
				int rvprows = gridparam.vis_row_end - gridparam.vis_row_start;
				int rvpcols = gridparam.vis_col_end - gridparam.vis_col_start;
				if (rx+ry!=0) {
					// recv
					MPI_Recv(uvistemp, rvprows*rvpcols, MPI_DOUBLE, rx*dims[1]+ry, 99, 
							 comm_2d, &status);
				}
				else {
					for (int i=0; i< rvprows*rvpcols; i++){
						uvistemp[i] = param.uvis[i];
					}
				}
				for (int i = gridparam.vis_row_start; i < gridparam.vis_row_end; i++){
					for (int j=gridparam.vis_col_start; j < gridparam.vis_col_end; j++){
						uvisfinal[i*(param.visres + 2) + j] = uvistemp[(i-gridparam.vis_row_start)*rvpcols+j-gridparam.vis_col_start];
					}
				}
			}
		}
		write_image(resfile, uvisfinal, param.visres + 2, param.visres + 2);
		free(uvisfinal); free(uvistemp);
	}
	else {
		//send
		MPI_Send(param.uvis, vprows*vpcols, MPI_DOUBLE, 0, 99,
				 comm_2d);
	}

	finalize(&param);

	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Finalize();

	return 0;
}
