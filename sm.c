#include "sm.h"
#include <stdlib.h>

sm_t *create_sm( int n, int nz ) {
	sm_t *m = malloc( sizeof *m );
	if( m == NULL )
		return NULL;
	if( (m->ia= malloc( (n+1)*sizeof * m->ia )) == NULL ) {
		free( m );
		return NULL;
	}
	if( (m->ja= malloc( nz*sizeof * m->ja )) == NULL ) {
		free( m->ia );
		free( m );
		return NULL;
	}
	if( (m->a= malloc( nz*sizeof * m->a )) == NULL ) {
		free( m->ja );
		free( m->ia );
		free( m );
		return NULL;
	}
	m->n = n;
	return m;
}

sm_t *read_sm( FILE *in ) {
	sm_t * m = malloc( sizeof * m);
	fscanf( in, "%d", &(m->n) );
	m->ia = malloc( (m->n+1) * sizeof * m->ia );
	int i;
	for( i= 0; i < m->n+1; i++ )
		fscanf( in, "%d", m->ia+i );
	int nz = m->ia[m->n];
	m->ja = malloc( nz * sizeof * m->ja );
	m->a = malloc( nz * sizeof * m->a );
	for( i= 0; i < nz; i++ )
		fscanf( in, "%d %lf", m->ja+i, m->a+i );
	return m;
}

void write_sm( sm_t *m, FILE *out) {
	fprintf( out, "%d\n", m->n );
	int i;
	for( i= 0; i < m->n+1; i++ )
		fprintf( out, "%d\n", m->ia[i] );
	for( i= 0; i < m->ia[m->n]; i++ )
		fprintf( out, "%d %g\n", m->ja[i], m->a[i] );
}

void sm_vec_dot( sm_t *m, double *x, double *y ) {
/* y = M*x */
	int i;
	for( i= 0; i < m->n; i++ ) {
		y[i] = 0;
		int j;
		for( j= m->ia[i]; j < m->ia[i+1]; j++ )
			y[i] += m->a[j]*x[m->ja[j]];
	}
}

int sm_put( sm_t *m, int i, int j, double a ) {
	int k;
	for( k= m->ia[i]; k < m->ia[i+1]; k++ )
    if( m->ja[k] == j ) {
			m->a[k] = a;
			return 1;
		}
	return 0;
}

int sm_add( sm_t *m, int i, int j, double a ) {
	int k;
	for( k= m->ia[i]; k < m->ia[i+1]; k++ )
    if( m->ja[k] == j ) {
			m->a[k] += a;
			return 1;
		}
	return 0;
}

double sm_get( sm_t *m, int i, int j ) {
	int k;
	for( k= m->ia[i]; k < m->ia[i+1]; k++ )
		if( m->ja[k] == j )
			return m->a[k];
	return 0.0;
}

void print_sm( sm_t *m, FILE *out ) {
	int i,j;
	int n = m->n;
	for( i= 0; i < n; i++ ) {
		for( j= 0; j < n; j++ )
			fprintf( out, "%010g ", sm_get( m, i, j ) );
		fprintf( out, "\n" );
	}
}

#include <string.h>
#include <math.h>

int sm_solve( sm_t *m, double *r, double *x, double tol, int maxit ) {
// Gauss-Seidel iterative solver
	int i;
	int n = m->n;
	double *xp = malloc( n * sizeof *xp );

	for( i= 0; i < maxit; i++ ) {
		memcpy( xp, x, n*sizeof *x );
		int j;
		for( j= 0; j < n; j++ ) {
			double ajj= 0.0;
			x[j]= r[j];
			int k;
			for( k= m->ia[j]; k < m->ia[j+1]; k++ ){
				if( m->ja[k] < j )
					x[j] -= m->a[k]*x[m->ja[k]];
				else if( m->ja[k] > j )
					x[j] -= m->a[k]*xp[m->ja[k]];
				else
					ajj = m->a[k];
			}
			x[j] /= ajj;
		}
		double err= 0.0;
		for( j= 0; j < n; j++ )
			err += (x[j]-xp[j])*(x[j]-xp[j]);
#ifdef DEBUG
		fprintf( stderr, "sm_solve: it: %d, err=%g\n", i, sqrt(err) );
#endif
		if( sqrt(err) < tol ) {
			free( xp );
			return i;
		}
	}
	free( xp );
	return maxit;
}

void free_sm( sm_t *m ) {
	free( m->ia );
	free( m->ja );
	free( m->a );
	free( m );
}




