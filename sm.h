#ifndef _SM_H_
#define _SM_H_

// Sparse matrix - Compressed Row Storage

#include <stdio.h>

typedef struct {
	int n;
	int *ia;
	int *ja;
	double *a;
} sm_t;

sm_t *create_sm( int n, int nz ); // n - # of equations, nz - # of nonzeros

sm_t *read_sm( FILE * );

void write_sm( sm_t *, FILE *);

void sm_vec_dot( sm_t *m, double *v, double *r );  // makes r = m * v

int sm_solve( sm_t *m, double *r, double *x, double tol, int maxit); // Gauss-Seidel iterative solves m * x = r with tolerance tol within maxit iterations

void print_sm( sm_t *, FILE * );

int sm_put( sm_t *, int, int, double );

int sm_add( sm_t *, int, int, double );

double sm_get( sm_t *, int, int );

void free_sm( sm_t * );

#endif
