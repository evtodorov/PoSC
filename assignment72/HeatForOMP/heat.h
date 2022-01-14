/*
 * heat.h
 *
 * Global definitions for the iterative solver
 */

#ifndef JACOBI_H_INCLUDED
#define JACOBI_H_INCLUDED

#include <stdio.h>
#include <mpi.h>
#include <omp.h>
// configuration

typedef struct
{
    float posx;
    float posy;
    float range;
    float temp;
}
heatsrc_t;

typedef struct
{
    unsigned maxiter;       // maximum number of iterations
    unsigned act_res;
    unsigned max_res;       // spatial resolution
    unsigned initial_res;
    unsigned res_step_size;
    unsigned visres;        // visualization resolution
  
    double *u, *uhelp;
    double *uvis;

    unsigned   numsrcs;     // number of heat sources
    heatsrc_t *heatsrcs;
}
algoparam_t;


typedef struct 
{
    // usage: for (i=*_*_start; i < *_*_end; ++i;)
    int grid_num_rows;     // number of rows in the grid
    int grid_num_cols;     // number of columns in the grid
    int grid_row;          // row of the current grid cell
    int grid_col;          // column of the current grid cell
    int store_row_start;   // start row for storage in this cell
    int compute_row_start; // start row for computation in this cell
    int store_col_start;   // start column for storage in this cell
    int compute_col_start; // start column for computation in this cell
    int store_row_end;     // end row for storage in this cell
    int compute_row_end;   // end row for computation in this cell
    int store_col_end;     // end column for storage in this cell
    int compute_col_end;   // end column for computation in this cell
    int vis_row_start;     // start row for visualization in this cell
    int vis_col_start;     // start column for visualization in this cell
    int vis_row_end;       // end row for visualization in this cell
    int vis_col_end;       // end column for visualization in this cell

}
gridparam_t;

// function declarations

// misc.c
int initialize( algoparam_t *param, gridparam_t *gridparam );
int finalize( algoparam_t *param );
void write_image( FILE * f, double *u,
		  unsigned sizex, unsigned sizey );
int coarsen(double *uold, unsigned oldx, unsigned oldy ,
	    double *unew, unsigned newx, unsigned newy );

void configure_grid(algoparam_t *algoparam, gridparam_t *gridparam);
void print_array(double *u, int sizex, int sizey);

// Gauss-Seidel: relax_gauss.c
double residual_gauss( double *u, double *utmp,
		       unsigned sizex, unsigned sizey );
void relax_gauss( double *u, 
		  unsigned sizex, unsigned sizey  );

// Jacobi: relax_jacobi.c
double residual_jacobi( double *u,
			unsigned sizex, unsigned sizey );
double relax_jacobi( double **u, double **utmp,
		   unsigned sizex, unsigned sizey ); 


#endif // JACOBI_H_INCLUDED
