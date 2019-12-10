#ifndef SOLVER_H
#define SOLVER_H

typedef struct {
	int n; // liczba wierszy
	int m; // liczba kolumn
	double **w;
} matrix_t;

double *solve( matrix_t h, double *r );

#endif
