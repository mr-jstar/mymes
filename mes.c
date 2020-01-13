#include "mes.h"
#include <stdlib.h>
#include <string.h>

int assemble( mesh_t *m, double *mat, double *src, sm_t **a, double **b ) {
	*a = make_sm_structure( m );
	*b = malloc( m->nodes->nn * sizeof **b );
	memset( (*a)->a, 0, (*a)->ia[(*a)->n]*sizeof *((*a)->a) );
	memset( *b, 0, m->nodes->nn*sizeof **b );
	int e,i,j,n,ig,jg;
  	double xe[3];
  	double ye[3];

  	double he[3][3];
  	double be[3];

	for( e= 0; e < m->elems->ne; e++ ) {
		for( j= 0; j < 3; j++ ) {
			n = m->elems->nop[3*e+j];
			xe[j]= m->nodes->xy[2*n];
			ye[j]= m->nodes->xy[2*n+1];
		}
		triangle3n( xe, ye, mat[e], src[e], he, be );
		for( i= 0; i < 3; i++ ) {
			ig = m->elems->nop[3*e+i];
			for( j= 0; j < 3; j++ ) {
					jg = m->elems->nop[3*e+j];
					sm_add( *a, ig, jg, he[i][j] );
			}
		  (*b)[ig] += be[i];
		}
	}
	return 0;
}

int dbc( sm_t *a, double *b, int i, double value ) {
	if( i < 0 || i > a->n )
		return -1;

	int j;
	for( j= a->ia[i]; j < a->ia[i+1]; j++ )
		if( a->ja[j] == i )
			a->a[j] = 1.0;
		else
			a->a[j] = 0.0;
	b[i] = value;

	return 0;
}

typedef struct node {
	int c;
	struct node *nxt;
} *c_list_t;

c_list_t add( c_list_t l, int c ) {
	if( l == NULL || l->c > c ) {
		c_list_t nn = malloc( sizeof *nn );
		nn->c = c;
		nn->nxt = l;
		return nn;
	} else if( l->c < c ) {
		l->nxt = add( l->nxt, c );
		return l;
	} else {
		return l;  /* will skip duplicates */
	}
}

int length( c_list_t l ) {
	int lgt= 0;
	while( l != NULL ) {
		lgt++;
		l = l->nxt;
	}
	return lgt;
}

void print_elem( int *nop, int e ) {
	printf( "elem%d = [ %d %d %d]\n", e, nop[3*e], nop[3*e+1], nop[3*e+2] );
}

sm_t *make_sm_structure( mesh_t *g ) {
	sm_t *m = malloc( sizeof *m );
	if( m == NULL )
		return NULL;
	m->n = g->nodes->nn;
	if( (m->ia = malloc((m->n+1)*sizeof *m->ia)) == NULL ) {
		free( m );
		return NULL;
	}
	c_list_t *rows = malloc( m->n * sizeof *rows );
	memset( rows, 0,  m->n * sizeof *rows  );
	int e,rv,cv,r,c;
	for( e= 0; e < g->elems->ne; e++ ) {
		for( rv= 0; rv < 3; rv++ ) {
		 	r = *(g->elems->nop+e*3+rv);
			if( r < 0 || r >= m->n ) { printf( "row # %d!\n", r ); print_elem( g->elems->nop, e ); }
			for( cv= 0; cv < 3; cv++ ) {
		 		c = *(g->elems->nop+e*3+cv);
				if( c < 0 || c >= m->n ) { printf( "column # %d!\n", c ); print_elem( g->elems->nop, e ); }
				rows[r] = add( rows[r], c );
			}
		}
	}
	int nz= 0;
	m->ia[0]= nz;
	for( r= 0; r < m->n; r++ ) {
		nz += length( rows[r] );
		m->ia[r+1] = nz;
	}
	if( (m->ja= malloc( nz * sizeof *m->ja )) == NULL ) {
		free( m->ia );
		free( m );
		return NULL;
	}
	if( (m->a= malloc( nz * sizeof *m->a )) == NULL ) {
		free( m->ja );
		free( m->ia );
		free( m );
		return NULL;
	}
	nz= 0;
	for( r= 0; r < m->n; r++ ) {
		c_list_t c= rows[r];
		while( c != NULL ) {
			m->ja[nz++]= c->c;
			c = c->nxt;
		}
	}
	for( r= 0; r < m->n; r++ ) {
		c_list_t c = rows[r];
		while( c != NULL ) {
			c_list_t tmp= c;
			c = c->nxt;
			free( tmp );
		}
	}
	free( rows );

	return m;
}

