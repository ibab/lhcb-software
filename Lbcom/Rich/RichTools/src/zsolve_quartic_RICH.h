
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
 *  $Id: zsolve_quartic_RICH.h,v 1.2 2005-06-20 16:27:12 jonrob Exp $
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

/** Solves the quartic equation x^4 + a x^3 + b x^2 + c x + d = 0
 *
 *  Optimised to give only solutions needed by RICH optical system
 *
 *  @return The solution needed by the RICH
 */
double solve_quartic_RICH ( const double a,
                            const double b,
                            const double c,
                            const double  );

#endif // RICHTOOLS_ZSOLVE_QUARTIC_RICH_H
