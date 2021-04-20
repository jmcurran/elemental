#include <math.h>
#include <errno.h>
#include <float.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

double lgamma(double x);

double d1mach(int flag)
{
	switch(flag)
	{
		case 1:
			return 1e-36;
		case 2:
			return 1e36;
	}
	return 2e-17;
}  /* d1mach */

double lbeta(double a, double b)
{
  return lgamma(a) + lgamma(b) - lgamma(a+b);
}

/*
 *  Hacked into C by Ross Ihaka from:
 *
 *  April 1977 Version.  W. Fullerton, C3, Los Alamos Scientific Lab.
 *  Based on Bosten and Battiste, Remark on Algorithm 179, Comm. Acm,
 *  V 17, p 153, (1974).
 *
 *  x   value to which function is to be integrated. x must be in (0,1).
 *  p   input (1st) parameter (must be greater than 0)
 *  q   input (2nd) parameter (must be greater than 0)
 *
 *  betai  Incomplete Beta Function Ratio
 *    The probability that a random variable from a beta
 *    distribution having parameters p and q will be less
 *    than or equal to x.
 */

double betai(double x,double pin,double qin)
{
  int i, ib, n;
  double c, finsum, p, p1, ps, q, term, xb, y;
  double betai;
  static double  eps = 0.0;
  static double  alneps = 0.0;
  static double  sml = 0.0;
  static double  alnsml = 0.0;

  if( eps==0.0 ) {
    eps = d1mach(3);
    sml = d1mach(1);
    alneps = log(eps);
    alnsml = log(sml);
  }
  
  if( x<0.0 || x>1.0 || pin<=0.0 || qin<=0.0 ) {
    errno = EDOM;
		return FLT_MAX;
  }

  y = x;
  p = pin;
  q = qin;
  if( q>p || x>=0.8 )
    if( x>=0.2 ) {
      y = 1.0-y;
      p = qin;
      q = pin;
    }
  
  if( (p+q)*y/(p+1.)<eps ) {
    betai = 0.0;
    xb = p*log(max(y,sml))-log(p)-lbeta(p,q);
    if( xb>alnsml &&y !=0.0 )
      betai = exp(xb);
    if( y!=x || p!=pin )
      betai = 1.0-betai;
  }
  else {
      /* evaluate the infinite sum first */

    ps = q-(int)q;
    if( ps==0.0 )
      ps = 1.0;
    xb = p*log(y)-lbeta(ps,p)-log(p);
    betai = 0.0;
    if( xb>=alnsml ) {
      betai = exp(xb);
      term = betai*p;
      if( ps!=1.0 ) {
        n = max(alneps/log(y),4.0);
        for( i=1 ; i<=n ; i++ ) {
          term = term*(i-ps)*y/i;
          betai = betai+term/(p+i);
        }
      }
    }

      /* now evaluate the finite sum, maybe */

    if( q>1.0 ) {
      xb = p*log(y)+q*log(1.0-y)-lbeta(p,q)-log(q);
      ib = max(xb/alnsml,0.0);
      term = exp(xb-ib*alnsml);
      c = 1.0/(1.0-y);
      p1 = q*c/(p+q-1.);
      finsum = 0.0;
      n = q;
      if( q==n )
        n = n-1;
      for( i=1 ; i<=n ; i++ ) {
        if( p1<=1.0 && term/eps<=finsum )
          break;
        term = (q-(i-1))*c*term/(p+q-i);
        if( term>1.0 )
          ib = ib-1;
        if( term>1.0 )
          term = term*sml;
        if( ib==0 )
          finsum = finsum+term;
      }
      betai = betai+finsum;
    }
    if( y!=x || p!=pin )
      betai = 1.0-betai;
    betai = max(min(betai,1.0),0.0);
  }
  return betai;
}

