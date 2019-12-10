#ifndef SOLVER_H
#define SOLVER_H

typedef struct {
	int n; // liczba wierszy
	int m; // liczba kolumn
	double **w;
} matrix_t;

matrix_t *create_matrix( int n, int m );

double get_entry( matrix_t *mat, int i, int j );

int set_entry( matrix_t *mat, int i, int j, double v ); 

int add_to_entry( matrix_t *mat, int i, int j, double v ); 

double * mul_mat_vec( matrix_t *mat, double *vec, double *res );

double *solve( matrix_t *h, double *r, double *x ); 

#endif
