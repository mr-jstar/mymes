#include "solver.h"
#include <stdlib.h>
#include <math.h>

matrix_t *create_matrix( int n, int m )
{
	matrix_t *mat = malloc( sizeof *mat );
	if( mat == NULL )
		return NULL;
	if( (mat->w = malloc( n * sizeof *(mat->w) )) == NULL ) {
		free( mat );
		return NULL;
	}
	if( (mat->w[0] = malloc( m*n * sizeof *(mat->w[0]))) == NULL ) {
		free( mat->w );
		free( mat );
		return NULL;
	}
	mat->n = n;
	mat->m = m;
	for( int i=1; i < n; i++ )
		mat->w[i] = mat->w[i-1] + m*sizeof *(mat->w[0]);
	return mat;
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

double * mul_mat_vec( matrix_t *mat, double *vec, double *res )
{
	for( int i= 0; i < mat->n; i++ ) {
		res[i] = 0;
		for( int j= 0; j < mat->m; j++ )
			res[i] += mat->w[i][j] * vec[j];
	}
	return res;
}

double *solve( matrix_t *h, double *r, double *x ) 
{
	// TO DO
	return x;
}
