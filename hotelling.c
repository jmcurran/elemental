#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stat.h"
#include "Mathlib.h"
#include "lr.h"
#include "wndproc.h"
#include <string.h>

void inv(double **a, double **ainv, int p);
double pf(double x, double n1, double n2);
void matprint(double **x, int nr, int nc)
{
  int i,j;

  for(i=0;i<nr;i++)
  {
    for(j=0;j<nc-1;j++)
      printf("%.2E ", x[i][j]);
    printf("%.2E\n", x[i][j]);
  }
}

void count_items(dbaseinfo *dbase, int *nx)
{
  FILE *fp;
  int n=0;
  char s[2000];

  fp=fopen(dbase->fname, "r");

  // skip first line

  fgets(s, 2000, fp);

  while(!feof(fp))
  {
    fgets(s, 2000, fp);
    n++;
  }
  n--;
  *nx=n;
}

void hotel(char *fname, char *result)
{

  FILE *fp;
  static int p, n1, n2, df1, df2;
  static double **x1, **x2, **s1, **s2, **sp, **spinv;
  static double *xbar1, *xbar2, *diff, *difft, *tmp;
  double pval, quant;
  double c;
  int i, j, k;
  double sum, TSQ;

  static double **X;
  
  fp=fopen(fname, "r");

  if(!fp)
  {
    sprintf(result, "Error: Could not open %s\n", fname);
    return;
  }

  /* read in the number of variables, and the number of observations in
     each sample */

  fscanf(fp,"%d,", &p);
  fscanf(fp,"%d,", &n1);
  fscanf(fp,"%d\n", &n2);

  if(n1+n2<=p+1)
  {
    sprintf(result,"Insufficient degrees of freedom - this could be solved by more\n control data\n");
    return;
  }

  x1=(double **)malloc(p*sizeof(double *));
  x2=(double **)malloc(p*sizeof(double *));

  xbar1=(double *)malloc(p*sizeof(double));
  xbar2=(double *)malloc(p*sizeof(double));
  diff=(double *)malloc(p*sizeof(double));
  difft=(double *)malloc(p*sizeof(double));
  tmp=(double *)malloc(p*sizeof(double));

  s1=(double **)malloc(p*sizeof(double *));
  s2=(double **)malloc(p*sizeof(double *));
  sp=(double **)malloc(p*sizeof(double *));
  spinv=(double **)malloc(p*sizeof(double *));

  for(i=0;i<p;i++)
  {
    x1[i]=(double *)malloc(n1*sizeof(double));
    x2[i]=(double *)malloc(n2*sizeof(double));
    s1[i]=(double *)malloc(p*sizeof(double));
    s2[i]=(double *)malloc(p*sizeof(double));
    sp[i]=(double *)malloc(p*sizeof(double));
    spinv[i]=(double *)malloc(p*sizeof(double));
  }

  for(i=0;i<n1+n2;i++)
  {
    for(j=0;j<p-1;j++)
    {
      if(i<n1)
        fscanf(fp, "%lf,", &x1[j][i]);
      else
        fscanf(fp, "%lf,", &x2[j][i-n1]);
    }
    if(i<n1)
      fscanf(fp, "%lf\n", &x1[j][i]);
    else
      fscanf(fp, "%lf\n", &x2[j][i-n1]);
  }

  fclose(fp);

  /* contrstuct the mean vectors and the difference of the means*/

  for(i=0;i<p;i++)
  {
    xbar1[i]=mean(x1[i], n1);
    xbar2[i]=mean(x2[i], n2);
    diff[i]=xbar1[i]-xbar2[i];
    difft[i]=xbar1[i]-xbar2[i];
  }

  /* find the sums of squares matrices for each sample */

  for(i=0;i<p;i++)
    for(j=0;j<p;j++)
    {
      s1[i][j]=0;
      s2[i][j]=0;
      if(i==j)
        spinv[i][j]=1;
      else
        spinv[i][j]=0;
      sp[i][j]=0;
    }
    

  for(k=0;k<n1;k++)
    for(i=0;i<p;i++)
      for(j=0;j<p;j++)
        s1[i][j]+=(x1[i][k]-xbar1[i])*(x1[j][k]-xbar1[j]);

  for(k=0;k<n2;k++)
    for(i=0;i<p;i++)
      for(j=0;j<p;j++)
        s2[i][j]+=(x2[i][k]-xbar2[i])*(x2[j][k]-xbar2[j]);

  for(i=0;i<p;i++)
    for(j=0;j<p;j++)
      sp[i][j]=((1.0/n1)+(1.0/n2))*(s1[i][j]+s2[i][j])/(n1+n2-2);

  /* now invert sp */

  inv(sp, spinv, p);

  /* and finally contruct T^2 */

  /* first premultiply spinv by diff */

  for(i=0;i<p;i++)
  {
    sum=0;
    for(j=0;j<p;j++)
      sum+=difft[j]*spinv[j][i];
    tmp[i]=sum;
  }
  for(i=0;i<p;i++)
    difft[i]=tmp[i];

  /* now find the inner product */

  sum=0;
  for(i=0;i<p;i++)
    sum+=difft[i]*diff[i];


  TSQ=sum;
  c=(n1+n2-2)*p*1.0/(n1+n2-p-1);
  df1=p;
  df2=n1+n2-p-1;
  pval=1-pf(TSQ/c,df1,df2);
  quant=c*qf(0.99,df1,df2);

  free(xbar1);
  free(xbar2);
  free(diff);
  free(difft);
  for(i=0;i<p;i++)
  {
    free(x1[i]);
    free(x2[i]);
    free(s1[i]);
    free(s2[i]);
    free(sp[i]);
    free(spinv[i]);
  }
  free(X);
  free(x1);
  free(x2);
  free(s1);
  free(s2);
  free(sp);
  free(spinv);

  if(pval<=0.001)
    sprintf(result, "%10.4lf   %10.4lf  %3d  %3d  <0.001\r\n\r\nThere is extremely strong evidence against the null hypothesis"
    , TSQ, quant, df1, df2);
  else if(pval>0.001&&pval<=0.01)
    sprintf(result, "%10.4lf   %10.4lf  %3d  %3d  %4.3lf\r\n\r\nThere is strong evidence against the null hypothesis"
    , TSQ, quant, df1, df2 , pval);
  else if(pval>0.01&&pval<=0.05)
    sprintf(result, "%10.4lf   %10.4lf  %3d  %3d  %4.3lf\r\n\r\nThere is evidence against the null hypothesis"
    , TSQ, quant, df1, df2 , pval);
  else if(pval>0.05&&pval<=0.1)
    sprintf(result, "%10.4lf   %10.4lf  %3d  %3d  %4.3lf\r\n\r\nThere is weak evidence against the null hypothesis"
      , TSQ, quant, df1, df2 , pval);
  else
    sprintf(result, "%10.4lf   %10.4lf  %3d  %3d  %4.3lf\r\n\r\nThere is no evidence against the null hypothesis"
    , TSQ, quant, df1, df2 , pval);
}
    
