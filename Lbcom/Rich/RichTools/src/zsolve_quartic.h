/* poly/zsolve_quartic.c
 *
 * Copyright (C) 2003 CERN and K.S. K\"{o}lbig
 *
 * Converted to C and implemented into the GSL Library - Sept. 2003
 * by Andrew W. Steiner and Andy Buckley
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* zsolve_quartic.c - finds the complex roots of
   x^4 + a x^3 + b x^2 + c x + d = 0
*/

#include <math.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_poly.h>

/* Prototypes -> gsl_poly.h header file */
int gsl_poly_complex_solve_quartic(const double a, const double b, const double c, const double d,
                                   gsl_complex * z0, gsl_complex * z1,
                                   gsl_complex * z2, gsl_complex * z3);
