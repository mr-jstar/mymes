#include "mesh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 2048  // assuming lines in .node and .ele not longer than that

nodes_t * free_nodes( nodes_t *nodes ) {
	free( nodes->xy );
	free( nodes->atr );
	free( nodes->mark );
	free( nodes );
	return NULL;
}

elems_t * free_elems( elems_t * elems ) {
	free( elems->nop );
	free( elems->atr );
	free( elems );
	return NULL;
}

int read_triangle_mesh( char *path, mesh_t *out ) {
// reads path.node and path.ele to out
// out can be NULL and is allocated dynamically in such case
// Contents of out is allocated dynamically in any case.
//
// Returns: 0 and filled out if success
//          1 - can't open .node or .ele
//          2 - not enough memory
//          31 - bad header of .node
//          41 - bad header of .ele
//          10+i - bad description of i-th node
//          -i - bad description of i-th elem

	char buf[BUF_SIZE];  // buffer used to read lines

	out->nodes = NULL;
	out->elems = NULL;  // just in case

	char *tmpstr = malloc( strlen(path)+6 );  // temporary string for filename constriction
	if( tmpstr == NULL )
		return 2;

	// read .node
	strcpy( tmpstr, path );
	strcat( tmpstr, ".node" );

	FILE *in = fopen( tmpstr, "r" );
	if( in == NULL ) {
		free( tmpstr );
		return 1;
	}
	
	if( (out->nodes = malloc( sizeof *(out->nodes) )) == NULL ) {
		free(tmpstr);
		return 2;
	}

	nodes_t *x = out->nodes; // temporary handle
	x->xy = NULL;
	x->atr = NULL;
	x->mark = NULL;

	// header line
	int tmpint;  // dummy variable used for some values to be skipped
	int bndind;  // used to read boundary marker indicator in headers

	while( fgets( buf, BUF_SIZE, in ) != NULL ) // skip empty lines and comments
		if( strlen(buf) > 1 && buf[0] != '#' )
			break;

	if( sscanf( buf, "%i %i %i %i", &x->nn, &tmpint, &x->n_atr ,&bndind ) != 4 || x->nn < 1 || tmpint != 2 || x->n_atr < 0 || bndind < 0 ) {
		// bad content of .node - clean up and exit
		out->nodes = free_nodes( out->nodes );
		free(tmpstr);
		fclose(in);
		return 31;
	}

	// allocate space for coordinates as well as for attributes and boundary markers if necessary
	if( (x->xy = malloc( x->nn * 2 * sizeof *(x->xy) )) == NULL ) {
		out->nodes = free_nodes( out->nodes );
		free(tmpstr);
		fclose(in);
		return 2;
	}
	if( x->n_atr == 0 )
		x->atr = NULL;
	else if( (x->atr = malloc( x->nn * x->n_atr * sizeof *(x->atr) )) == NULL ) {
		out->nodes = free_nodes( out->nodes );
		free(tmpstr);
		fclose(in);
		return 2;
	}
	if( bndind == 0 )
		x->mark = NULL;
	else if( (x->mark = malloc( x->nn * sizeof *(x->mark) )) == NULL ) {
		out->nodes = free_nodes( out->nodes );
		free(tmpstr);
		fclose(in);
		return 2;
	}

	int nn_start = -1; // to indicate wheter noe numbers start with 0 or 1
	// now read nodes
	for( int i= 0; i < x->nn; i++ ) {
		while( fgets( buf, BUF_SIZE, in ) != NULL ) // skip empty lines and comments
                	if( strlen(buf) > 1 && buf[0] != '#' )
                        	break;
		int pos; // to mark position in buffer, where attributes begin
		char *read_buf = buf;
		int nn;
		if( sscanf( buf, "%d %lf %lf%n", &nn, x->xy+2*i, x->xy+2*i+1, &pos ) != 3 ) { // note, that %n is not counted for output!
			out->nodes = free_nodes( out->nodes );
			fclose( in );
			free(tmpstr);
			return 10+i;
		}
		if( nn_start == -1 ) {
			nn_start = nn;
		}
		read_buf += pos;
		if( x->n_atr > 0 ) { // read nodal attributes
			for( int j= 0; j < x->n_atr; j++ ) {
				if( sscanf( read_buf, "%lf%n", x->atr+(i*x->n_atr)+j, &pos ) != 1 ) {
					out->nodes = free_nodes(out->nodes);
					free(tmpstr);
					fclose(in);
					return 3;
				}
				read_buf += pos;
			}
		}
		if( x->mark != NULL ) { // read nodal marker
			if( sscanf( read_buf, "%d", x->mark+i ) != 1 ) {
                        	out->nodes = free_nodes(out->nodes);
                                free(tmpstr);
                                fclose(in);
                                return 10+i;
                        }
		}
	}
	fclose(in);

	// read .ele
	strcpy( tmpstr, path );
	strcat( tmpstr, ".ele" );

	in = fopen( tmpstr, "r" );
	if( in == NULL ) {
		free( tmpstr );
		return 1;
	}
	
	if( (out->elems = malloc( sizeof *(out->elems) )) == NULL ) {
		free(tmpstr);
		return 2;
	}

	elems_t* e = out->elems; // temporary handle
	e->nop = NULL;
	e->atr = NULL;

	// header line

	while( fgets( buf, BUF_SIZE, in ) != NULL ) // skip empty lines and comments
		if( strlen(buf) > 1 && buf[0] != '#' )
			break;

	if( sscanf( buf, "%i %i %i", &e->ne, &tmpint, &e->n_atr ) != 3 || e->ne < 1 || tmpint != 3 || x->n_atr < 0 ) {
		// bad content of .ele - clean up and exit
		out->elems = free_elems( out->elems );
		free(tmpstr);
		fclose(in);
		return 41;
	}

	// allocate space for node-numbers as well as for attributes if necessary
	if( (e->nop = malloc( e->ne * 3 * sizeof *(e->nop) )) == NULL ) {
		out->elems = free_elems( out->elems );
		free(tmpstr);
		fclose(in);
		return 2;
	}
	if( e->n_atr == 0 )
		e->atr = NULL;
	else if( (e->atr = malloc( e->ne * e->n_atr * sizeof *(e->atr) )) == NULL ) {
		out->elems = free_elems( out->elems );
		free(tmpstr);
		fclose(in);
		return 2;
	}

	// now read elems
	for( int i= 0; i < e->ne; i++ ) {
		while( fgets( buf, BUF_SIZE, in ) != NULL ) // skip empty lines and comments
                	if( strlen(buf) > 1 && buf[0] != '#' )
                        	break;
		int pos; // to mark position in buffer, where attributes begin
		char *read_buf = buf;
		if( sscanf( buf, "%*d %d %d %d%n", e->nop+3*i, e->nop+3*i+1, e->nop+3*i+2, &pos ) != 3 ) { // note, that %n is not counted for output!
			out->elems = free_elems( out->elems );
			fclose( in );
			free(tmpstr);
			return -i;
		}
		// decrease each node number by one if node numbering generated by triangle starts with 1
		*(e->nop+3*i) -= nn_start;
		*(e->nop+3*i+1) -= nn_start;
		*(e->nop+3*i+2) -= nn_start;
		read_buf += pos;
		if( e->n_atr > 0 ) { // read attributes of elements
			for( int j= 0; j < e->n_atr; j++ ) {
				if( sscanf( read_buf, "%lf%n", e->atr+(i*e->n_atr)+j, &pos ) != 1 ) {
					out->elems = free_elems(out->elems);
					free(tmpstr);
					fclose(in);
					return -i;
				}
				read_buf += pos;
			}
		}
	}

	fclose(in);
	free(tmpstr);
	return 0;
}




