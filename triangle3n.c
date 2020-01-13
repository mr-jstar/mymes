#include "mes.h"

void triangle3n( double *x, double *y, double e, double f, double he[][3], double *re ) {
/* macierz elementu trójkątnego o wierzchołkach x,y */
	double a[3], b[3];
	double s = 0.5*( (x[1]*y[2]-x[2]*y[1]) - (x[0]*y[2]-x[2]*y[0]) + (x[0]*y[1]-x[1]*y[0]) );
	a[0] = ( y[1]-y[2] ) / (2*s);
	a[1] = ( y[2]-y[0] ) / (2*s);
	a[2] = ( y[0]-y[1] ) / (2*s);
	b[0] = ( x[2]-x[1] ) / (2*s);
	b[1] = ( x[0]-x[2] ) / (2*s);
	b[2] = ( x[1]-x[0] ) / (2*s);

	int i,j;

	for( i= 0; i < 3; i++ ) {
		for( j= 0; j < 3; j++ )
			he[i][j] = ( a[i]*a[j] + b[i]*b[j] ) * e * s;
		re[i] = f*s/3;
	}
}
