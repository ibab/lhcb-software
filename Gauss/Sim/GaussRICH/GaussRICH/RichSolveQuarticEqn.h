#ifndef GAUSSRICH_SolveQuarticEqn_H
#define GAUSSRICH_SolveQuarticEqn_H 1


/** @file RichSolveQuarticEqn.h
 *
 *  zsolve_quartic.c - finds the complex roots of
 *   x^4 + a x^3 + b x^2 + c x + d = 0
 *
 *  CVS Log :-
 *  $Id: RichSolveQuarticEqn.h,v 1.1 2005-05-09 12:25:36 seaso Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.2  2004/07/26 18:03:05  jonrob
 *  Various improvements to the doxygen comments
 *
 *
 *  name changed to RichSolveQuarticEqn to avoid any name clashes.
 *  In the future this should be changed back to zsolve_quartic
 *  SE 10-5-2005.
 *  @todo switch to GSL version when available
 */

#include <math.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_poly.h>

/* Prototypes -> gsl_poly.h header file */
int gsl_poly_complex_solve_quartic_eqn ( const double a, const double b, const double c, const double d,
                                     gsl_complex * z0, gsl_complex * z1,
                                     gsl_complex * z2, gsl_complex * z3);


#endif // GAUSSRICH_SolveQuarticEqn
