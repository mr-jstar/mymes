#include "solver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define TEST_SIZE 10

int main( int argc, char **argv ) {
	int n = argc > 1 ? atoi( argv[1] ) : TEST_SIZE;
	matrix_t *m = create_matrix( n, n );
	double r[n];
	double x[n];

	if( m == NULL ) {
		fprintf( stderr, "%s: create_matrix not working\n", argv[0] );
		return 1;
	}

	srand( time(NULL) );
	for( int i= 0; i < n; i++ ) {
		x[i] = i;
		for( int j= 0; j < n; j++ ) {
			if( set_entry( m, i, j, 0.0 ) != 0 ) {
				fprintf( stderr, "%s: set_entry not working\n", argv[0] );
				return 1;
			}
			if( get_entry( m, i, j ) != 0.0 ) {
				fprintf( stderr, "%s: get_entry not working\n", argv[0] );
				return 1;
			}
			if( add_to_entry( m, i, j, (double)rand()/RAND_MAX ) != 0 ) {
				fprintf( stderr, "%s: add_to_entry not working\n", argv[0] );
				return 1;
			}
		}
	}
	for( int i= 0; i < n; i++ )
		set_entry( m, i, i, 100*get_entry( m, i, i ) );

	mul_mat_vec( m, x, r );

	memset( x, 0, n * sizeof *x );

	solve( m, r, x );

	for( int i= 0; i < n; i++ )
		if( fabs(x[i]-i) > 1e-10 ) {
			fprintf( stderr, "%s: mul_mat_vec or solve not working\n", argv[0] );
			return 1;
		}

	delete_matrix( m );

	return 0;
}




