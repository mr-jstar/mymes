#include "solver.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

matrix_t *create_matrix( int n, int m ) // create empty matrix given size
{
	matrix_t *mat = malloc( sizeof *mat );
	if( mat == NULL )
		return NULL;
	if( (mat->w = malloc( n * sizeof *(mat->w) )) == NULL ) {
		free( mat );
		return NULL;
	}
	if( (mat->w[0] = calloc( m*n, sizeof *(mat->w[0]))) == NULL ) {
		free( mat->w );
		free( mat );
		return NULL;
	}
	mat->n = n;
	mat->m = m;
	for( int i=1; i < n; i++ )
		mat->w[i] = mat->w[i-1] + m;
	return mat;
}

void delete_matrix( matrix_t *mat ) // free memory
{
	free( mat->w[0] );
	free( mat->w );
	free( mat );
}

double get_entry( matrix_t *mat, int i, int j )
{
	if( i >= 0 && i < mat->n && j >= 0 && j < mat->m ) {
		return mat->w[i][j];
	} else
		return NAN;
}

int set_entry( matrix_t *mat, int i, int j, double v ) 
{
	if( i >= 0 && i < mat->n && j >= 0 && j < mat->m ) {
		mat->w[i][j] = v;
		return 0;
	} else
		return 1;
}

int add_to_entry( matrix_t *mat, int i, int j, double v ) 
{
	if( i >= 0 && i < mat->n && j >= 0 && j < mat->m ) {
		mat->w[i][j] += v;
		return 0;
	} else 
		return 1;
}

void mul_mat_vec( matrix_t *mat, double *vec, double *rslt )
{
	for( int i= 0; i < mat->n; i++ ) {
		rslt[i] = 0;
		for( int j= 0; j < mat->m; j++ )
			rslt[i] += mat->w[i][j] * vec[j];
	}
}

static void vec_minus( double *x, double *y, int n ) {
	for( int i= 0; i < n; i++ )
		x[i] -= y[i];
}

#include <stdio.h>
static double vec_L2( double *x, int n ) {
	double sum = 0.0;
	for( int i= 0; i < n; i++ )
		sum += x[i]*x[i];
	printf( "sum=%g\n", sum );
	return sqrt( sum );
}

static int iter_solver( matrix_t *h, double *r, double *x, double prec, int maxiter, int *iter_done, double *res ) {
	// Simplest Jacobi iterative solver
	// Solves h x = r, with precision prec ( ||x-xp|| <= x
	// takes argument x as an initial solution and returns the final solution in x
	// if iter_done is not NULL it will return the number of iterations done
	// if res is not NULL it will contain the residuals (||h x_i - r||)
	int n = h->n;
	double *xp = malloc( n * sizeof *xp );
	int it;
	double res_i = 0.0;
	for( int i= 0; i < n; i++ )
		if( fabs(get_entry( h, i, i )) < 1e-12 )
			printf( "zero at diag %d,%d\n", i, i );
	for( it= 0; it < maxiter; it++ ) {
		memcpy( xp, x, n * sizeof *xp );
		mul_mat_vec( h, xp, x );  
		for( int j= 0; j < n; j++ )
			printf( "xp[%d]=%g, ax[%d]= %g\n", j, xp[j], j, x[j] );
		printf( "\n" );
		vec_minus( x, r, n );  // store ( h x_p - r ) in x
		for( int j= 0; j < n; j++ )
			printf( "res[%d]=%g  ", j, x[j] );
		printf( "\n" );
		for( int j= 0; j < n; j++ ) {
			x[j] = xp[j] - x[j] / get_entry( h, j, j );
			printf( "x[%d]=%g ", j, x[j] );
		}
		printf( "\n" );
		vec_minus( xp, x, n );
		res_i = vec_L2( xp, n );
		printf( "it: %d |deltax|=%g\n", it, res_i );
		if( res != NULL )
			res[it] = res_i;
		if( res_i <= prec ) {
			break;
		}
	}
	if( iter_done != NULL )
		*iter_done = it;
	free( xp );
	return res_i > prec;   // 0 == success, 1 == fail
}


int solve( matrix_t *h, double *r, double *x ) 
{
	int iter;
	int fail= iter_solver( h, r, x, 1e-12, 10*h->n, &iter, NULL );
	printf( "Jacobi did %d iterations result=%s\n", iter, (fail?"FAIL":"success") );
	return fail;
}
