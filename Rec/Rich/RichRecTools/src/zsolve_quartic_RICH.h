
//----------------------------------------------------------------------
/** @file zsolve_quartic_RICH.h
 *
 *  Finds the two real roots of x^4 + a x^3 + b x^2 + c x + d = 0
 *
 *  Hacked version that is no longer purely generic, but is optimised for
 *  the solutions needed for the RICH system (i.e. purely real)
 *
 *  Compare to gsl_poly_complex_solve_quartic in zsolve_quartic.cpp
 *  for more details on how this works
 *
 *  @see gsl_poly_complex_solve_quartic
 *
 *  CVS Log :-
 *  $Id: zsolve_quartic_RICH.h,v 1.3 2006-10-26 14:27:16 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-14
 */
//----------------------------------------------------------------------

#ifndef RICHTOOLS_ZSOLVE_QUARTIC_RICH_H
#define RICHTOOLS_ZSOLVE_QUARTIC_RICH_H 1

#include <math.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_poly.h>

//----------------------------------------------------------------------
/** Solves the quartic equation x^4 + a x^3 + b x^2 + c x + d = 0
 *
 *  Optimised to give only solutions needed by RICH optical system
 *
 *  @return The solution needed by the RICH
 */
//----------------------------------------------------------------------
inline double solve_quartic_RICH ( const double a,
                                   const double b,
                                   const double c,
                                   const double d )
{
  const double r4 = 1.0 / 4.0;
  const double q2 = 1.0 / 2.0;
  const double q8 = 1.0 / 8.0;
  const double q1 = 3.0 / 8.0;
  const double q3 = 3.0 / 16.0;

  const double aa = a * a;
  const double pp = b - q1 * aa;
  const double qq = c - q2 * a * (b - r4 * aa);
  const double rr = d - r4 * (a * c - r4 * aa * (b - q3 * aa));
  const double rc = q2 * pp;
  const double sc = r4 * (r4 * pp * pp - rr);
  const double tc = -(q8 * qq * q8 * qq);

  const double qcub = (rc * rc - 3 * sc);
  const double rcub = (2 * rc * rc * rc - 9 * rc * sc + 27 * tc);

  const double Q = qcub / 9;
  const double R = rcub / 54;

  const double Q3 = Q * Q * Q;
  const double R2 = R * R;

  const double sgnR = ( R >= 0 ? 1 : -1 );
  //const double A = -sgnR * cbrt( fabs(R) + sqrt(R2 - Q3) );
  const double A = -sgnR * pow( (fabs(R) + sqrt(R2 - Q3)), 1/3 );
  const double B = Q / A;

  const double u1 = -0.5 * (A + B) - rc / 3;
  const double u2 = -( sqrt(3.0) / 2.0) * fabs(A - B);

  gsl_complex w1, w2, w3;
  GSL_SET_COMPLEX(&w1, u1, u2);
  GSL_SET_COMPLEX(&w2, u1, -u2);
  GSL_SET_COMPLEX(&w3, 0.0, 0.0);
  w1=gsl_complex_sqrt(w1);
  w2=gsl_complex_sqrt(w2);

  if ( gsl_complex_abs(gsl_complex_mul(w1, w2)) != 0.0 )
  {
    w3 = gsl_complex_mul_real(gsl_complex_inverse(gsl_complex_mul(w1, w2)), -qq / 8.0);
  }

  return GSL_REAL(w1) + GSL_REAL(w2) + GSL_REAL(w3) - (r4*a);

}

#endif // RICHTOOLS_ZSOLVE_QUARTIC_RICH_H
