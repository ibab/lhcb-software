
/** @file zsolve_quartic.h
 *
 *  zsolve_quartic.c - finds the complex roots of
 *   x^4 + a x^3 + b x^2 + c x + d = 0
 *
 *  CVS Log :-
 *  $Id: zsolve_quartic.h,v 1.2 2004-07-26 18:03:05 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @todo switch to GSL version when available
 */

#include <math.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_poly.h>

/* Prototypes -> gsl_poly.h header file */
int gsl_poly_complex_solve_quartic ( const double a, const double b, const double c, const double d,
                                     gsl_complex * z0, gsl_complex * z1,
                                     gsl_complex * z2, gsl_complex * z3);
