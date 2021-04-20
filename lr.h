#include <math.h>
#include <stdio.h>

void sort(double *x, int n);
double dnorm(double x);
double fhat(double X, double *x, double sigma, int nx);
void lin_comb(double *x, double *l, double **DB, int nx, int p);
void find_lin_comb(double *l, double **sp, double *dif, int p, int n1, int n2);
