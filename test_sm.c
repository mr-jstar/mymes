#include "sm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define TEST_SIZE 10

int main( int argc, char **argv ) {
	int n = argc > 1 ? atoi( argv[1] ) : TEST_SIZE;

	double r[n];
	double x[n];

	// we will create a 5-diag matrix: number of nonzeros: n+2*(n-2)+2*(n-4)
	sm_t *m = create_sm( n, n+2*(n-1)+2*(n-2) );
	printf( "%d equations, %d nonzeros\n", n, n+2*(n-1)+2*(n-2) );
	if( m == NULL ) {
		fprintf( stderr, "%s: create_matrix not working\n", argv[0] );
		return 1;
	}

	srand( time(NULL) );
	int ne = 0;
	for( int i= 0; i < n; i++ ) {
		m->ia[i] = ne;
		for( int j= -2; j <=2 ; j++ ) {
			if( i+j >= 0 && i+j < n ) {
				m->ja[ne++] = i+j;
			}
		}
	}
	m->ia[n] = ne;
	for( int i= 0; i < n; i++ ) {
		x[i] = i;
		for( int j= -2; j <=2 ; j++ ) {
			if( i+j >= 0 && i+j < n ) {
				if( sm_put( m, i, i+j, (double)rand()/RAND_MAX ) != 1 ) {
					fprintf( stderr, "%s failed to put entry (%d,%d)\n", argv[0], i, i+j );
					print_sm( m, stderr );
					write_sm( m, stderr );
					return 1;
				}
			}
		}
	}
	for( int i= 0; i < n; i++ )
		sm_put( m, i, i, 100*sm_get( m, i, i ) );

	if( n <= 10 ) {
		printf( "Matrix:\n" );
		print_sm( m, stdout );
	}

	sm_vec_dot( m, x, r );

	memset( x, 0, n * sizeof *x );

	sm_solve( m, r, x, 1e-12, n );

	for( int i= 0; i < n; i++ ) {
		if( fabs(x[i]-i) > 1e-10 ) {
			fprintf( stderr, "%s: mul_mat_vec or solve not working\n", argv[0] );
			return 1;
		}
	}

	if( n <= 10 ) {
		printf( "Solution:\n" );
		for( int i= 0; i < n; i++ ) {
			printf( "%g\n", x[i] );
		}
	}

	free_sm( m );

	return 0;
}




