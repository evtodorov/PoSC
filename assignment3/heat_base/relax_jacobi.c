/*
 * relax_jacobi.c
 *
 * Jacobi Relaxation
 *
 */

#include "heat.h"

#define BLOCK_SIZE 256

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
/*
 * Residual (length of error vector)
 * between current solution and next after a Jacobi step
 */
double residual_jacobi(double *u, unsigned sizex, unsigned sizey) {
	unsigned i, j;
	double unew, diff, sum = 0.0;

	for (int by=0; by < sizey/BLOCK_SIZE; by++){
		for (int bx=0; bx < sizex/BLOCK_SIZE; bx++){
			for (i = max(1, by*BLOCK_SIZE); i < min((by+1)*BLOCK_SIZE, sizey - 1); i++){
				for (j = max(1, bx*BLOCK_SIZE); j < min((bx+1)*BLOCK_SIZE, sizex - 1); j++) {
					unew = 0.25 * (u[i * sizex + (j - 1)] +  // left
								u[i * sizex + (j + 1)] +  // right
								u[(i - 1) * sizex + j] +  // top
								u[(i + 1) * sizex + j]); // bottom

					diff = unew - u[i * sizex + j];
					sum += diff * diff;
				}
			}
		}
	}


	return sum;
}

/*
 * One Jacobi iteration step
 */
void relax_jacobi(double* restrict u, double * restrict utmp, unsigned sizex, unsigned sizey) {
	int i, j;

	for (int by=0; by < sizey/BLOCK_SIZE; by++){
		for (int bx=0; bx < sizex/BLOCK_SIZE; bx++){
			for (i = max(1, by*BLOCK_SIZE); i < min((by+1)*BLOCK_SIZE, sizey - 1); i++){
				for (j = max(1, bx*BLOCK_SIZE); j < min((bx+1)*BLOCK_SIZE, sizex - 1); j++) {
					utmp[i * sizex + j] = 0.25 * (u[i * sizex + (j - 1)] +  // left
												u[i * sizex + (j + 1)] +  // right
												u[(i - 1) * sizex + j] +  // top
												u[(i + 1) * sizex + j]); // bottom
				}
			}
		}
	}
}
