#include "mes.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define MAX_BC 64
#define MAX_MAT 64

int main( int argc, char **argv ) {
	int bon[MAX_BC];     /* assigned indicators of boundary conditions */
	double bv[MAX_BC];  /* values of boundary conditions */
	double sf[MAX_MAT]; /* sources */
	double mc[MAX_MAT]; /* material coefficients */

	char *plotname= NULL;
	FILE *plot = NULL;
	FILE *out = NULL;
	mesh_t m;

	char c;
	int bi,ai;
	double v,s;
	int err_c;
	int run_gnuplot = 0;
	int mats_used = 0;

	m.nodes = NULL;
	m.elems = NULL;

	memset( bon, 0, MAX_BC*sizeof *bon );
	memset( bv, 0, MAX_BC*sizeof *bv );
	memset( sf, 0, MAX_MAT*sizeof *sf );
	memset( mc, 0, MAX_MAT*sizeof *mc );

  opterr = 0;

  while ((c = getopt (argc, argv, "hb:i:m:o:p:g")) != -1) {
    switch (c)
      {
			case 'h':
				printf( "Usage:\n"
					"\t%s [-b <b#>:<bv>]... [-m <m#>:<cv>:<sv>]... [-i<mesh-name>] [-o<output>] [-p<plot>]\n"
					"\t\twhere:\n"
					"\t\t-b -- Dirichlet boundary condition: b# - boundary number, bv - value\n"
					"\t\t-m -- material specification: m# - subregion number, cv - material coefficient, sv - source\n"
					"\t\t  -b and -m may be used more than once\n"
					"\t\t-i -- core name of the mesh (<mesh-name>.node and <mesh-name>.ele expected)\n"
					"\t\t-o -- write calculated values to <output>\n"
					"\t\t-p -- write plot in gluplot format to <plot>\n"
					"\t\t-g -- run gnuplot at the end\n"
					, argv[0] );
				exit( EXIT_SUCCESS );
				break;
      case 'b':
        if( sscanf( optarg, "%d:%lf", &bi, &v ) != 2 ) {
					fprintf( stderr, "%s: bad arguments of -b switch\n", argv[0] );
					return EXIT_FAILURE;
				}
				if( bi >= MAX_BC ) {
					fprintf( stderr, "%s: sorry - only %d boundaries are allowed\n", argv[0], MAX_BC );
					return EXIT_FAILURE;
				}
				bon[bi] = 1;
				bv[bi] = v;
        break;
      case 'i':
        if(  (err_c= read_triangle_mesh( optarg, &m )) != 0 ) {
					fprintf( stderr, "%s: bad argument of -i switch: can't read triangle output from %s, error #= %d\n", argv[0], optarg, err_c );
					return EXIT_FAILURE;
				} 
        break;
			case 'o':
				if( optarg != NULL ) {
					if( (out = fopen(optarg,"w")) == NULL ) {
						fprintf( stderr, "%s: can't open %s for writing!", argv[0], optarg );
						return EXIT_FAILURE;
					}
				} else {
					out = stdout;
				}
				break;
			case 'p':
				if( optarg != NULL ) {
					plotname = optarg;
					if( (plot = fopen(optarg,"w")) == NULL ) {
						fprintf( stderr, "%s: can't open %s for writing!", argv[0], optarg );
						return EXIT_FAILURE;
					}
				} else {
					plotname = NULL;
					plot = stdout;
				}
        break;
      case 'm':
        if( sscanf( optarg, "%d:%lf:%lf", &ai, &v, &s ) != 3 ) {
					fprintf( stderr, "%s: bad arguments of -m switch\n", argv[0] );
					return EXIT_FAILURE;
				}
				if( ai >= MAX_MAT ) {
					fprintf( stderr, "%s: sorry - only %d sub-domains are allowed\n", argv[0], MAX_MAT );
					return EXIT_FAILURE;
				}
				mc[ai] = v;
				sf[ai] = s;
				mats_used = 1;
#ifdef DEBUG
				printf( "mat[%d]= { %g, %g }\n", ai, mc[ai], sf[ai] );
#endif
        break;
			case 'g': 
				run_gnuplot = 1;
				break;
      default:
        fprintf( stderr, "%s: unknown switch -%c (ignored)\n", argv[0], c );
				break;
      }
	}
	if( m.nodes == NULL || m.elems == NULL ) {
		fprintf( stderr, "%s: nothing to do - no mesh defined\n", argv[0] );
		return EXIT_FAILURE;
	}
	if( m.nodes->mark == NULL ) {
		fprintf( stderr, "%s: no boundary indicators defined in mesh - can't apply any DBC!\n", argv[0] );
		return EXIT_FAILURE;
	}

	double *mat= malloc( m.elems->ne * sizeof *mat );
	double *src= malloc( m.elems->ne * sizeof *src );
	int i;
	if( m.elems->n_atr != 1 ) {
		if( mats_used )
			fprintf( stderr, "%s: no structure (element attributes) defined - materials will be ignored (if used)!\n", argv[0] );
		for( i= 0; i < m.elems->ne; i++ ) {
			mat[i] = 1;
			src[i] = 0;
		}
	} else {
		for( i= 0; i < m.elems->ne; i++ ) {
			mat[i] = mc[ (int)m.elems->atr[i] ];
			src[i] = sf[ (int)m.elems->atr[i] ];
		}
	}
	sm_t *a = NULL;
	double *b = NULL;
	if( assemble( &m, mat, src, &a, &b ) == 0 ) {

#ifdef DEBUG
		printf( "Assembled matrix:\n---\n" );
		print_sm( a, stdout );
		printf( "---\n" );
#endif
		
		for( i= 0; i < m.nodes->nn; i++ ) 
			if( bon[ m.nodes->mark[i] ] != 0 )
				dbc( a, b, i, bv[ m.nodes->mark[i] ] );
#ifdef DEBUG
		print_sm( a, stdout );
#endif

		double *u = calloc( m.nodes->nn, sizeof *u );
		printf( "System solved in %d iterations\n", sm_solve( a, b, u, 1e-3, m.nodes->nn ) );
		if( out != NULL ) {
			fprintf( out, "%d\n", m.nodes->nn );
			for( i= 0; i < m.nodes->nn; i++ )
				fprintf( out, "%g\n", u[i] );
		}
		if( plot != NULL ) {
	    for( i= 0; i < m.elems->ne; i++ ) {
				int j,n;
	      for( j= 0; j< 3; j++ ) {
					n = m.elems->nop[3*i+j];
					fprintf( plot, "%g %g %g\n", m.nodes->xy[2*n], m.nodes->xy[2*n+1], u[n] );
				}
				n = m.elems->nop[3*i];
				fprintf( plot, "%g %g %g\n\n\n", m.nodes->xy[2*n], m.nodes->xy[2*n+1], u[n] );
	    }
			fclose( plot );
			if( run_gnuplot && plotname != NULL ) {
				char cmd[1024];
				sprintf( cmd, "set multiplot layout 1,2; splot '%s' w l; pause 5; quit", plotname );
				execlp( "gnuplot", "gnuplot", "-e", cmd, (char*)NULL );
			}
		}
		return EXIT_SUCCESS;
	} else {
		fprintf( stderr, "%s: Problems in LinEqSystem assembly\n", argv[0] );
		return EXIT_FAILURE;
	}
}
