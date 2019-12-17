#include "mesh.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc == 2) {
	mesh_t mesh;
	int res;
    	if( (res= read_triangle_mesh(argv[1], &mesh)) != 0 ) {
		printf( "Mesh reader failed, err code = %d (see mesh.c for explanation)\n", res );
		return 1;
	}
	if( mesh.nodes->nn <= 0 ) {
		printf( "Mesh reader failed, not positive # of nodes(%d)\n", mesh.nodes->nn );
		return 2;
	}
	if( mesh.elems->ne <= 0 ) {
		printf( "Mesh reader failed, not positive # of elements(%d)\n", mesh.elems->ne );
		return 3;
	}
    } else {
        printf("NOT TESTED: You have to specify core of the mesh files path!\n");
	return 4;
    }
    return 0;
}
