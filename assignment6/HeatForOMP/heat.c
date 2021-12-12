#include <stdio.h>
#include <stdlib.h>

#include "input.h"
#include "heat.h"
#include "timing.h"

double* time;

void usage(char *s) {
	fprintf(stderr, "Usage: %s <input file> [result file]\n\n", s);
}

int main(int argc, char *argv[]) {
	int i, j, k, ret;
	FILE *infile, *resfile;
	char *resfilename;
	int np, iter, chkflag;

	
	// algorithmic parameters
	algoparam_t param;

	// timing

	double residual;

	// set the visualization resolution
	param.visres = 100;

	// check arguments
	if (argc < 2) {
		usage(argv[0]);
		return 1;
	}

	// check input file
	if (!(infile = fopen(argv[1], "r"))) {
		fprintf(stderr, "\nError: Cannot open \"%s\" for reading.\n\n", argv[1]);

		usage(argv[0]);
		return 1;
	}

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

	// configure grid NB doesn't do any memory allocation; usage TBD
	gridparam_t gridparam;
	gridparam.grid_num_rows = 3; // TODO: get correct number from config
	gridparam.grid_num_cols = 4; // TODO: get correct number from config
	gridparam.grid_row = 1; // TODO: get correct row from MPI
	gridparam.grid_col = 2; // TODO: get correct col from MPI
	configure_grid(&param, &gridparam);

	printf("\nCompute: \u<=row<\u, \u<=col<\u\nStore: \u<=row<\u, \u<=col<\u\n", 
		    gridparam.compute_row_start,
	 		gridparam.compute_row_end,
			gridparam.compute_col_start,
			gridparam.compute_col_end,
			gridparam.store_row_start,
	 		gridparam.store_row_end,
			gridparam.store_col_start,
			gridparam.store_col_end);


	print_params(&param);
	time = (double *) calloc(sizeof(double), (int) (param.max_res - param.initial_res + param.res_step_size) / param.res_step_size);

	int exp_number = 0;

	for (param.act_res = param.initial_res; param.act_res <= param.max_res; param.act_res = param.act_res + param.res_step_size) {
		if (!initialize(&param)) {
			fprintf(stderr, "Error in Jacobi initialization.\n\n");

			usage(argv[0]);
		}

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

		printf("\n\nResolution: %u\n", param.act_res);
		printf("===================\n");
		printf("Execution time: %f\n", time[exp_number]);
		printf("Residual: %f\n\n", residual);

		printf("megaflops:  %.1lf\n", (double) param.maxiter * (np - 2) * (np - 2) * 7 / time[exp_number] / 1000000);
		printf("  flop instructions (M):  %.3lf\n", (double) param.maxiter * (np - 2) * (np - 2) * 7 / 1000000);

		exp_number++;
	}

	param.act_res = param.act_res - param.res_step_size;

	coarsen(param.u, param.act_res + 2, param.act_res + 2, param.uvis, param.visres + 2, param.visres + 2);

	write_image(resfile, param.uvis, param.visres + 2, param.visres + 2);

	finalize(&param);
	return 0;
}
