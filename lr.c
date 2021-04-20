#include <math.h>
#include <stdio.h>
#include <malloc.h>
#include "stat.h"

void inv(double **a, double **ainv, int p);

void sort(double *x, int n)
{
  double xtmp;
  int h=1,i,j;

  /* Code starts here */

  do
  {
    h=3*h+1;
  }while(h<=n);

  do
  {
    h=h/3;
    for(i=h;i<n;++i)
    {
      xtmp=x[i];
      j=i-h;
      while(j>=0&&xtmp<x[j]&&j<=n)
      {
        x[j+h]=x[j];
        j=j-h;
      }
      x[j+h]=xtmp;
    }
  }while(h>=1);
}

double dnorm(double x)
{
  double result;
  double pi=4*atan(1);

  result=exp(-0.5*x*x)/sqrt(2*pi);

  return result;
}

double fhat(double y, double *x, double sigma, int nx)
{
  double h;
  double c;
  int i;
  double sum=0;
  double xi;

  if(y>x[nx-1])
    y=(x[nx-1]+x[nx-2]+x[nx-3])/3.0;
  if(y<x[0])
    y=(x[0]+x[1]+x[2])/3.0;

  h=1.06*sigma*pow(nx, -0.2);

  c=h*nx;

  for(i=0;i<nx;i++)
  {
    xi=(y-x[i])/h;
    sum+=dnorm(xi);
  }

  return sum/c;
}

void lin_comb(double *x, double *l, double **DB, int nx, int p)
// multiplies the matrix X by the linear combination l and squares it
// DB is nx by p
// x in nx by 1
// l is p by 1
{
  int i, j;

  for(i=0;i<nx;i++)
  {
    x[i]=0;
    for(j=0;j<p;j++)
      x[i]+=l[j]*DB[i][j];
    x[i]*=x[i];
  }
}

void find_lin_comb(double *l, double **sp, double *dif, int p, int n1, int n2)
{
  double c;
  int i, j;
  double **tmp, **spinv;

  c=(1.0/n1)+(1.0/n2);

  tmp=(double **)malloc(p*sizeof(double *));
  spinv=(double **)malloc(p*sizeof(double *));
  for(i=0;i<p;i++)
  {
    tmp[i]=(double *)malloc(p*sizeof(double));
    spinv[i]=(double *)malloc(p*sizeof(double));
    for(j=0;j<p;j++)
      tmp[i][j]=sp[i][j]/c;
  }

  inv(tmp, spinv, p);

  for(i=0;i<p;i++)
  {
    l[i]=0;
    for(j=0;j<p;j++)
      l[i]+=spinv[i][j]*dif[j];
  }

  for(i=0;i<p;i++)
  {
    free(spinv[i]);
    free(tmp[i]);
  }

  free(spinv);
  free(tmp);
}

