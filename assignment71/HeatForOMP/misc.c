/*
 * misc.c
 *
 * Helper functions for
 * - initialization
 * - finalization,
 * - writing out a picture
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <omp.h>

#include "heat.h"

#define min(a,b) (((a)<(b))?(a):(b))

/*
 * Initialize the iterative solver
 * - allocate memory for matrices
 * - set boundary conditions according to configuration
 */
int initialize( algoparam_t *param, gridparam_t *gridparam )
{
    int i, j;
    double dist;

    // total number of points (including border)
	const int nptot = param->act_res + 2;

	// local number of points
    const int nprows = gridparam->store_row_end - gridparam->store_row_start;
	const int npcols = gridparam->store_col_end - gridparam->store_col_start;
    const int vprows = gridparam->vis_row_end - gridparam->vis_row_start;
	const int vpcols = gridparam->vis_col_end - gridparam->vis_col_start;
    //
    // allocate memory
    //
    (param->u)     = (double*)malloc( sizeof(double)* nprows*npcols );
    (param->uhelp) = (double*)malloc( sizeof(double)* nprows*npcols );
    (param->uvis)  = (double*)calloc( sizeof(double), vprows*vpcols );

    for (i=0;i<nprows;i++){
    	for (j=0;j<npcols;j++){
    		param->u[i*npcols+j]=0;
			param->uhelp[i*npcols+j]=0;
    	}
    }

    if( !(param->u) || !(param->uhelp) || !(param->uvis) )
    {
	fprintf(stderr, "Error: Cannot allocate memory\n");
	return 0;
    }

    for( i=0; i<param->numsrcs; i++ )
    {
	/* top row */
	if (gridparam->grid_row==0){
	for( j=0; j<nptot; j++ )
	{
	    dist = sqrt( pow((double)j/(double)(nptot-1) -
			     param->heatsrcs[i].posx, 2)+
			 pow(param->heatsrcs[i].posy, 2));

	    if( dist <= param->heatsrcs[i].range )
	    {
			if( j >= gridparam->store_col_start &&
			    j <  gridparam->store_col_end){
				(param->u)[j-gridparam->store_col_start] +=
					(param->heatsrcs[i].range-dist) /
					param->heatsrcs[i].range *
					param->heatsrcs[i].temp;
			}
	    }
	}
	}
	/* bottom row */
	if (gridparam->grid_row==gridparam->grid_num_rows-1){
	for( j=0; j<nptot; j++ )
	{
	    dist = sqrt( pow((double)j/(double)(nptot-1) -
			     param->heatsrcs[i].posx, 2)+
			 pow(1-param->heatsrcs[i].posy, 2));

	    if( dist <= param->heatsrcs[i].range )
	    {
			if( j >= gridparam->store_col_start &&
			    j <  gridparam->store_col_end){
				(param->u)[(gridparam->store_col_end - 2)*npcols+j-gridparam->store_col_start]+=
					(param->heatsrcs[i].range-dist) /
					param->heatsrcs[i].range *
					param->heatsrcs[i].temp;
			}

	    }
	}
	}
	/* leftmost column */
	if (gridparam->grid_col==0){
	for( j=1; j<nptot-1; j++ )
	{
	    dist = sqrt( pow(param->heatsrcs[i].posx, 2)+
			 pow((double)j/(double)(nptot-1) -
			     param->heatsrcs[i].posy, 2));

	    if( dist <= param->heatsrcs[i].range )
	    {
			if( j >= gridparam->store_row_start &&
			    j <  gridparam->store_row_end){
				(param->u)[ (j-gridparam->store_row_start)*npcols ]+=
					(param->heatsrcs[i].range-dist) /
					param->heatsrcs[i].range *
					param->heatsrcs[i].temp;
			}

	    }
	}
	}
	/* rightmost column */
	if (gridparam->grid_col==gridparam->grid_num_cols-1){
	for( j=1; j<nptot; j++ )
	{
	    dist = sqrt( pow(1-param->heatsrcs[i].posx, 2)+
			 pow((double)j/(double)(nptot-1) -
			     param->heatsrcs[i].posy, 2));

	    if( dist <= param->heatsrcs[i].range )
	    {
			if( j >= gridparam->store_row_start &&
			    j <  gridparam->store_row_end){
				(param->u)[ (j-gridparam->store_row_start)*npcols+(npcols-1) ]+=
					(param->heatsrcs[i].range-dist) /
					param->heatsrcs[i].range *
					param->heatsrcs[i].temp;
			}
	    }
	}
	}
    }

    return 1;
}

/*
 * Configure the grid boundaries based
*/
void configure_grid(algoparam_t *algoparam, gridparam_t *gridparam){
    // points excluding the border
	int compute_points = algoparam->act_res;
	// total number of points (including border)
	int store_points = compute_points + 2;
	// total visualisation points
	int vis_points = algoparam->visres + 2;

	int num_rows = gridparam->grid_num_rows;
	int num_cols = gridparam->grid_num_cols;
	int row = gridparam->grid_row;
	int col = gridparam->grid_col;
	int row_stride = (compute_points + num_rows - 1)/num_rows;
	int col_stride = (compute_points + num_cols - 1)/num_cols;
	int vis_row_stride = (vis_points + num_rows - 1)/num_rows;
	int vis_col_stride = (vis_points + num_cols - 1)/num_cols;

	(gridparam->store_row_start) = row * row_stride;
	(gridparam->store_col_start) = col * col_stride;
	(gridparam->vis_row_start) = row * vis_row_stride;
	(gridparam->vis_col_start) = col * vis_col_stride;
	(gridparam->compute_row_start) = (gridparam->store_row_start) + 1;
	(gridparam->compute_col_start) = (gridparam->store_col_start) + 1;

	(gridparam->store_row_end) = min((row+1)*row_stride+1, store_points);
	(gridparam->store_col_end) = min((col+1)*col_stride+1, store_points);
	(gridparam->store_row_end) = min((row+1)*vis_row_stride+1, vis_points);
	(gridparam->store_col_end) = min((col+1)*vis_col_stride+1, vis_points);
	(gridparam->compute_row_end) = (gridparam->store_row_end) - 1;
	(gridparam->compute_col_end) = (gridparam->store_col_end) - 1;



}

/*
 * free used memory
 */
int finalize( algoparam_t *param )
{
    if( param->u ) {
	free(param->u);
	param->u = 0;
    }

    if( param->uhelp ) {
	free(param->uhelp);
	param->uhelp = 0;
    }

    if( param->uvis ) {
	free(param->uvis);
	param->uvis = 0;
    }

    return 1;
}


/*
 * write the given temperature u matrix to rgb values
 * and write the resulting image to file f
 */
void write_image( FILE * f, double *u,
		  unsigned sizex, unsigned sizey )
{
    // RGB table
    unsigned char r[1024], g[1024], b[1024];
    int i, j, k;

    double min, max;

    j=1023;

    // prepare RGB table
    for( i=0; i<256; i++ )
    {
	r[j]=255; g[j]=i; b[j]=0;
	j--;
    }
    for( i=0; i<256; i++ )
    {
	r[j]=255-i; g[j]=255; b[j]=0;
	j--;
    }
    for( i=0; i<256; i++ )
    {
	r[j]=0; g[j]=255; b[j]=i;
	j--;
    }
    for( i=0; i<256; i++ )
    {
	r[j]=0; g[j]=255-i; b[j]=255;
	j--;
    }

    min=DBL_MAX;
    max=-DBL_MAX;

    // find minimum and maximum
    for( i=0; i<sizey; i++ )
    {
	for( j=0; j<sizex; j++ )
	{
	    if( u[i*sizex+j]>max )
		max=u[i*sizex+j];
	    if( u[i*sizex+j]<min )
		min=u[i*sizex+j];
	}
    }


    fprintf(f, "P3\n");
    fprintf(f, "%u %u\n", sizex, sizey);
    fprintf(f, "%u\n", 255);

    for( i=0; i<sizey; i++ )
    {
	for( j=0; j<sizex; j++ )
	{
	    k=(int)(1024.0*(u[i*sizex+j]-min)/(max-min));
	    fprintf(f, "%d %d %d  ", r[k], g[k], b[k]);
	}
	fprintf(f, "\n");
    }
}

int coarsen( double *uold, unsigned oldx, unsigned oldy ,
	     double *unew, unsigned newx, unsigned newy )
{
    int i, j, k, l, ii, jj;

    int stopx = newx;
    int stopy = newy;
    float temp;
    float stepx = (float) oldx/(float)newx;
    float stepy = (float)oldy/(float)newy;

    if (oldx<newx){
	 stopx=oldx;
	 stepx=1.0;
    }
    if (oldy<newy){
     stopy=oldy;
     stepy=1.0;
    }

    //printf("oldx=%d, newx=%d\n",oldx,newx);
    //printf("oldy=%d, newy=%d\n",oldy,newy);
    //printf("rx=%f, ry=%f\n",stepx,stepy);
    // NOTE: this only takes the top-left corner,
    // and doesnt' do any real coarsening

    for( i=0; i<stopy; i++ ){
       ii=stepy*i;
       for( j=0; j<stopx; j++ ){
          jj=stepx*j;
          temp = 0;
          for ( k=0; k<stepy; k++ ){
	       	for ( l=0; l<stepx; l++ ){
	       		if (ii+k<oldx && jj+l<oldy)
		           temp += uold[(ii+k)*oldx+(jj+l)] ;
	        }
	      }
	      unew[i*newx+j] = temp / (stepy*stepx);
       }
    }

  return 1;
}
