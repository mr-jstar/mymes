#ifndef SOLVER_H
#define SOLVER_H

typedef struct {
	int n; // # of rows
	int m; // # of columns
	double **w;
} matrix_t;

matrix_t *create_matrix( int n, int m );

void delete_matrix( matrix_t * );

double get_entry( matrix_t *mat, int i, int j );

int set_entry( matrix_t *mat, int i, int j, double v ); 

int add_to_entry( matrix_t *mat, int i, int j, double v ); 

void mul_mat_vec( matrix_t *mat, double *vec, double *res );

int solve( matrix_t *h, double *r, double *x ); 

#endif
