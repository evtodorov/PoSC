/*
 * relax_jacobi.c
 *
 * Jacobi Relaxation
 *
 */

#include "heat.h"

double relax_jacobi(double *u, double *utmp, unsigned sizex, unsigned sizey) {
	int i, j;
	double unew, diff, sum = 0.0;

	for (j = 1; j < sizex - 1; j++) {
		for (i = 1; i < sizey - 1; i++) {
			double u_initial =  u[i * sizex + j];
			utmp[i * sizex + j] = 0.25 * (u[i * sizex + (j - 1)] +  // left
						u[i * sizex + (j + 1)] +  // right
						u[(i - 1) * sizex + j] +  // top
						u[(i + 1) * sizex + j]); // bottom

			diff = u_initial - utmp[i * sizex + j];
			sum += diff * diff; // Residual calculation here
		}
	}
	// utmp stores the u for the next step, whereas the residual is calculated for the current step.
	// In practice, this would result in one extra iteration after the residual has reached the threshold.

	return sum;
}

