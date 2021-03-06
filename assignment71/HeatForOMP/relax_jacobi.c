/*
 * relax_jacobi.c
 *
 * Jacobi Relaxation
 *
 */

#include "heat.h"
#include <stdlib.h>


double relax_jacobi( double **u1, double **utmp1,
         unsigned sizex, unsigned sizey, MPI_Status *status, MPI_Request *request, MPI_Comm* comm_2d)
{
  int i, j;
  double *help,*u, *utmp,factor=0.5;

  utmp=*utmp1;
  u=*u1;
  double unew, diff, sum=0.0;

	int rank, up, down, left, right;
  int coord[2];
  int nprows = sizey;
  int npcols = sizex;
  double *firstcolumn, *secondcolumn, *lastcolumn, *secondtolastcolumn;
  firstcolumn = (double*)malloc( sizeof(double)* nprows );
  lastcolumn = (double*)malloc( sizeof(double)* nprows );
  secondcolumn = (double*)malloc( sizeof(double)* nprows );
  secondtolastcolumn = (double*)malloc( sizeof(double)* nprows );
  MPI_Comm_rank(*comm_2d, &rank);
	MPI_Cart_coords(*comm_2d, rank, 2, coord);
	MPI_Cart_shift(*comm_2d, 0, 1, &up, &down);
	MPI_Cart_shift(*comm_2d, 1, 1, &left, &right);

  // compute ghost cells
  // second and secondtolast rows
  for( i=1; i<sizey-1; i+=sizey-3 ) {
  	int ii=i*sizex;
  	int iim1=(i-1)*sizex;
  	int iip1=(i+1)*sizex;
#pragma ivdep
    for( j=1; j<sizex-1; j++ ){
       unew = 0.25 * (u[ ii+(j-1) ]+
        		            u[ ii+(j+1) ]+
        		            u[ iim1+j ]+
        		            u[ iip1+j ]);
		    diff = unew - u[ii + j];
		    utmp[ii+j] = unew;
		    sum += diff * diff;

       }
    }
    // second and secondtolast columns
  for( i=1; i<sizey-1; i++ ) {
  	int ii=i*sizex;
  	int iim1=(i-1)*sizex;
  	int iip1=(i+1)*sizex;
    for( j=1; j<sizex-1; j+=sizex-3 ){
       unew = 0.25 * (u[ ii+(j-1) ]+
        		            u[ ii+(j+1) ]+
        		            u[ iim1+j ]+
        		            u[ iip1+j ]);
		    diff = unew - u[ii + j];
		    utmp[ii+j] = unew;
		    sum += diff * diff;
       }
    }



  // send ghost cells
  for (int i=0; i < nprows; i++){
      secondcolumn[i] = utmp[i*npcols+1]; 
      secondtolastcolumn[i] = utmp[(i+1)*npcols-2];
  }
	if (left!=-1){
      MPI_Isend(secondcolumn, nprows, MPI_DOUBLE, left, 10,
						*comm_2d, &request[0]); // send left
      MPI_Irecv(firstcolumn, nprows, MPI_DOUBLE, left, 20,
						*comm_2d, &request[4]); // recv left
  }
  else {
    request[0] = MPI_REQUEST_NULL;
    request[4] = MPI_REQUEST_NULL;
  }
  if (right!=-1){ 
    	MPI_Isend(secondtolastcolumn, nprows, MPI_DOUBLE, right, 20,
						*comm_2d, &request[1]);  // send right
      MPI_Irecv(lastcolumn, nprows, MPI_DOUBLE, right, 10,
							*comm_2d, &request[5]); // recv right
  }  else {
    request[1] = MPI_REQUEST_NULL;
    request[5] = MPI_REQUEST_NULL;
  }
  if (up!=-1){
      MPI_Isend((utmp+npcols), npcols, MPI_DOUBLE, up, 30,
							*comm_2d, &request[2]); // send up
      MPI_Irecv(utmp, npcols, MPI_DOUBLE, up, 40, 
								*comm_2d, &request[6]); // recv up
  }  else {
    request[2] = MPI_REQUEST_NULL;
    request[6] = MPI_REQUEST_NULL;
  }          
  if (down!=-1){
	    MPI_Isend((utmp+(nprows-2)*npcols), npcols, MPI_DOUBLE, down, 40,
							*comm_2d, &request[3]);
      MPI_Irecv((utmp+(nprows-1)*npcols), npcols, MPI_DOUBLE, down, 30, 
							*comm_2d, &request[7]);
  }  else {
    request[3] = MPI_REQUEST_NULL;
    request[7] = MPI_REQUEST_NULL;
  }
    
  // compute the rest

  for( i=2; i<sizey-2; i++ ) {
  	int ii=i*sizex;
  	int iim1=(i-1)*sizex;
  	int iip1=(i+1)*sizex;
#pragma ivdep
    for( j=2; j<sizex-2; j++ ){
       unew = 0.25 * (u[ ii+(j-1) ]+
        		            u[ ii+(j+1) ]+
        		            u[ iim1+j ]+
        		            u[ iip1+j ]);
		    diff = unew - u[ii + j];
		    utmp[ii+j] = unew;
		    sum += diff * diff;

       }
    }


  // wait until all send and recv is completed
  MPI_Waitall(8, request, status); 
  if (right!=-1){
    for (int i=0; i < nprows; i++){
			utmp[(i+1)*npcols-1] = lastcolumn[i]; 
		} 
  }
  if (left!=-1){
    for (int i=0; i < nprows; i++){
      utmp[i*npcols] = firstcolumn[i];
    } 
  }

  *u1=utmp;
  *utmp1=u;
  free(firstcolumn); free(lastcolumn); free(secondcolumn); free(secondtolastcolumn);
  return(sum);
}


