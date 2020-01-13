#ifndef _MES_H_
#define _MES_H_

#include "sm.h"
#include "mesh.h"

int assemble( mesh_t * m, double *mat, double *s, sm_t **a, double **b );

int dbc( sm_t *a, double *b, int i, double value );

void triangle3n( double *x, double *y, double e, double f,
                 double he[][3], double *re );

sm_t *make_sm_structure( mesh_t * );

#endif
