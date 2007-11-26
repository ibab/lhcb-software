
//----------------------------------------------------------------------
/** @file zsolve_quartic.h
 *
 *  Finds the complex roots of x^4 + a x^3 + b x^2 + c x + d = 0
 *
 *  CVS Log :-
 *  $Id: zsolve_quartic.h,v 1.1.1.1 2007-11-26 17:25:47 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-14
 *
 *  @todo switch to GSL version if and when available
 */
//----------------------------------------------------------------------

#ifndef RICHTOOLS_ZSOLVE_QUARTIC_H
#define RICHTOOLS_ZSOLVE_QUARTIC_H 1

#include <math.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_poly.h>

/** Solves the quartic equation x^4 + a x^3 + b x^2 + c x + d = 0
 *
 *  @param a 
 *  @param b
 *  @param c
 *  @param d
 *  @param z0 First complex solution
 *  @param z1 Second complex solution
 *  @param z2 Third complex solution
 *  @param z3 Fourth complex solution
 *
 *  @return Number of solutions found
 */
int gsl_poly_complex_solve_quartic ( const double a, 
                                     const double b, 
                                     const double c, 
                                     const double d,
                                     gsl_complex * z0, 
                                     gsl_complex * z1,
                                     gsl_complex * z2, 
                                     gsl_complex * z3 );

#endif // RICHTOOLS_ZSOLVE_QUARTIC_H
