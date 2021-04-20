#include <stdio.h>
#include <math.h>

void lu(double **a, int *swap, int p)
{
	double big, tmp;
	int i, j, k, l, stmp;
	double lambda;

	/* initialise swap */
	for(i=0;i<p;i++)
		swap[i]=i;

	for(k=0;k<p-1;k++)
	{
		/* find pivot */
		big=0;
		for(i=k;i<p;i++)
			if(fabs(a[i][k])>big)
			{
				big=fabs(a[i][k]);
				l=i;
			}

		if(l!=k)
		{
			stmp=swap[k];
			swap[k]=swap[l];
			swap[l]=stmp;
			for(j=0;j<p;j++)
			{
				tmp=a[k][j];
				a[k][j]=a[l][j];
				a[l][j]=tmp;
			}
		}

		for(i=k+1;i<p;i++)
		{
			lambda=a[i][k]/a[k][k];
			for(j=k;j<p;j++)
				a[i][j]=a[i][j]-(lambda*a[k][j]);
			a[i][k]=lambda;
		}
	}
}

void lusolve(double **a, double *b, int *swap, int p)
{
	int i, j, k;
	double sum;
	int *tmp;

	/* sort b */
	tmp=(int *)malloc(p*sizeof(int));
	for(i=0;i<p;i++)
		tmp[i]=b[swap[i]];
	for(i=0;i<p;i++)
		b[i]=tmp[i];
	free(tmp);
	/* forward elimination first */

	for(i=0;i<p;i++)
	{
		sum=b[i];
		for(j=0;j<i;j++)
			sum-=a[i][j]*b[j];
		b[i]=sum;
	}

	/* backward substitution */

	k=p-1;

	while(k>=0)
	{
		sum=b[k];
		for(i=k+1;i<p;i++)
			sum-=a[k][i]*b[i];
		b[k]=sum/a[k][k];
		k--;
	}
}


void matmult(double **a, double **b, double **c, int n)
{
	int i, j, k;

	for(i=0;i<n;i++)
		for(j=0;j<n;j++)
		{
			c[i][j]=0;
			for(k=0;k<n;k++)
				c[i][j]+=a[i][k]*b[k][j];
		}
}


void inv(double **a, double **ainv, int p)
{
	int *idx;
	int i,j, k;
	double d;
	double *b;
	double **tmp;

	tmp=(double **)malloc(p*sizeof(double *));
	b=(double *)malloc(p*sizeof(double));
	idx=(int *)malloc(p*sizeof(int));


	for(i=0;i<p;i++)
	{
		tmp[i]=(double *)malloc(p*sizeof(double));
		for(j=0;j<p;j++)
			tmp[i][j]=a[i][j];
	}


	lu(tmp, idx, p);
	for(k=0;k<p;k++)
	{
		for(i=0;i<p;i++)
		{
			if(i==k)
				b[i]=1;
			else
				b[i]=0;	
		}
		
		lusolve(tmp, b, idx, p);

		for(i=0;i<p;i++)
			ainv[i][k]=b[i];
	}


	for(i=0;i<p;i++)
		free(tmp[i]);
	free(tmp);
	free(b);
	free(idx);
}

