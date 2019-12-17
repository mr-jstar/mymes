#ifndef MESH_H
#define MESH_H

typedef struct nodes {
        int nn; // # of nodes
        double *xy; // coords: x0,y0, x1,y1, x2,y2 and so on
        int n_atr; // # of attributes in each node
        double *atr; // nodal attributes (NULL if none), attributes are stored in the same maner as coordinates: a01,a02,..a0n_atr, a11,a12,...
        int *mark; // nodal markers (NULL if none) - just one marker per node
} nodes_t;

typedef struct elems {
        int ne; // # of elems (3N triangles)
        int *nop; // n01,n02,n03, n11,n12,n13 and so on
        int n_atr; // # of attributes for each elem
        double *atr; // elem attributes (NULL if none) - see nodal attr. above for explanation
} elems_t;

typedef struct {
        nodes_t *nodes;
        elems_t *elems;
} mesh_t;

int read_triangle_mesh( char *path, mesh_t *out );  // reads path.node and path.ele to out

#endif
