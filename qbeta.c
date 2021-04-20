/*
 *  R : A Computer Langage for Statistical Data Analysis
 *  Copyright (C) 1995  Robert Gentleman and Ross Ihaka
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* Reference:
 * Cran, G. W., K. J. Martin and G. E. Thomas (1977).
 * Remark AS R19 and Algorithm AS 109,
 * Applied Statistics, 26, 111-114.
 */


#include "Mathlib.h"

static double zero = 0.0;
static double half = 0.5;
static double one = 1.0;
static double two = 2.0;
static double three = 3.0;
static double four = 4.0;
static double five = 5.0;
static double six = 6.0;
static double nine = 9.0;

/* set the exponent of accu to -2r-2 for r digits of accuracy */
static double acu = 1.0e-32;
static double lower = 0.0001;
static double upper = 0.9999;
static double const1 = 2.30753;
static double const2 = 0.27061;
static double const3 = 0.99229;
static double const4 = 0.04481;

double qbeta(double alpha, double p, double q)
{
  int index;
  double a, adj, beta, g, h, pp, prev, qq, r, s, sq, t, tx, w,
   y, yprev;
  double xinbta;

  /* define accuracy and initialize */

  xinbta = alpha;

  /* test for admissibility of parameters */

  if (p < zero || q < zero || alpha < 0.0 || alpha > 1.0) {
    errno = EDOM;
    return -DBL_MAX;
  }
  if (alpha == zero || alpha == one)
    return alpha;

  beta = exp(lbeta(p, q));

  /* change tail if necessary */

  if (alpha <= half) {
    a = alpha;
    pp = p;
    qq = q;
    index = 0;
  } else {
    a = one - alpha;
    pp = q;
    qq = p;
    index = 1;
  }

  /* calculate the initial approximation */

  r = sqrt(-log(a * a));
  y = r - (const1 + const2 * r) / (one + (const3 + const4 * r) * r);
  if (pp > one && qq > one) {
    r = (y * y - three) / six;
    s = one / (pp + pp - one);
    t = one / (qq + qq - one);
    h = two / (s + t);
    w = y * sqrt(h + r) / h - (t - s) * (r + five / six - two / (three * h));
    xinbta = pp / (pp + qq * exp(w + w));
  } else {
    r = qq + qq;
    t = one / (nine * qq);
    t = r * pow(one - t + y * sqrt(t), 3.0);
    if (t <= zero)
      xinbta = one - exp((log((one - a) * qq) + beta) / qq);
    else {
      t = (four * pp + r - two) / t;
      if (t <= one)
        xinbta = exp((log(a * pp) + beta) / pp);
      else
        xinbta = one - two / (t + one);
    }
  }

  /* solve for x by a modified newton-raphson method, */
  /* using the function betain */

  r = one - pp;
  t = one - qq;
  yprev = zero;
  sq = one;
  prev = one;
  if (xinbta < lower)
    xinbta = lower;
  if (xinbta > upper)
    xinbta = upper;
  for (;;) {
    y = pbeta(xinbta, pp, qq);
    if (errno)
      break;
    y = (y - a) * exp(beta + r * log(xinbta) + t * log(one - xinbta));
    if (y * yprev <= zero)
      prev = sq;
    g = one;
    for (;;) {
      adj = g * y;
      sq = adj * adj;
      if (sq < prev) {
        tx = xinbta - adj;
        if (tx >= zero && tx <= one) {
          if (prev <= acu)
            goto L10;
          if (y * y <= acu)
            goto L10;
          if (tx != zero && tx != one)
            break;
        }
      }
      g = g / three;
    }
    if (tx == xinbta)
      goto L10;
    xinbta = tx;
    yprev = y;
  }
  errno = ERANGE;
  return -HUGE_VAL;
      L10:
  if (index)
    xinbta = one - xinbta;
  return xinbta;
}
