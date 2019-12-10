#ifndef SOLVER_H
#define SOLVER_H

typedef struct {
	int n; // liczba wierszy
	int m; // liczba kolumn
	double **w;
} matrix_t;

matrix_t *create_matrix( int n, int m );

int set_entry( matrix_t *m, int i, int j, double v );

int add_to_entry( matrix_t *m, int i, int j, double v );

double *solve( matrix_t h, double *r );

#endif
