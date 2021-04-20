#include <math.h>

double mean(double *x, int nx)
{
  int n;
  double c, xn, xn1;

  xn=x[0];

  for(n=0;n<(nx-1);n++)
  {
    c=1.0*n+1.0;
    xn1=xn+((x[n+1]-xn)/(c+1));
    xn=xn1;
  }

  return xn;
}

double bgss(double *y1, double *y2, int n1, int n2)
{
  double ybar2, ybar1;

  ybar1=mean(y1,n1);
  ybar2=mean(y2,n2);

  return n1*n2*(ybar1-ybar2)*(ybar1-ybar2)/(n1+n2);
}

double sum_of_squares(double *x, int nx)
{
  int n;
  double c, sn, sn1, xn;

  sn=0;
  xn=x[0];

  for(n=0;n<(nx-1);n++)
  {
    c=(n+1)*1.0;
    sn1=sn+(c/(c+1)*(xn-x[n+1])*(xn-x[n+1]));
    sn=sn1;
    xn=xn+((x[n+1]-xn)/(c+1));
  }

  return sn1;
}

double var(double *x, int nx)
{
  double c;

  c=((double)(nx-1));

  return sum_of_squares(x,nx)/c;
}

double mle_var(double *x, int nx)
{
  double c;

  c=((double)(nx));

  return sum_of_squares(x,nx)/c;
}


