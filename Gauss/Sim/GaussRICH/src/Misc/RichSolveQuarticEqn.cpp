
/**  @file RichSolveQuarticEqn.cpp
 *
 *  Implementation file for RichSolveQuarticEqn.h
 *
 *  CVS Log :-
 *  $Id: RichSolveQuarticEqn.cpp,v 1.1 2005-05-09 12:25:36 seaso Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.2  2004/07/26 18:03:05  jonrob
 *  Various improvements to the doxygen comments
 *
 *  Name changed to RichSolveQuarticEqn to avoid name clashes
 *  and dependencies.
 *  Eventually this should be changed back to zsolve_quartic
 *  to avoid duplication. SE 10-5-2005.
 *  @todo switch to GSL version when available
 */

#include "GaussRICH/RichSolveQuarticEqn.h"

#define SWAP(a,b) do { gsl_complex tmp = b ; b = a ; a = tmp ; } while(0)

int gsl_poly_complex_solve_quartic_eqn(const double a, const double b, const double c, const double d,
                                   gsl_complex * z0, gsl_complex * z1,
                                   gsl_complex * z2, gsl_complex * z3)
{
  const double r4 = 1.0 / 4.0;
  const double q2 = 1.0 / 2.0;
  const double q4 = 1.0 / 4.0;
  const double q8 = 1.0 / 8.0;
  const double q1 = 3.0 / 8.0;
  const double q3 = 3.0 / 16.0;
  double u[3], v[3], disc;
  int k1 = 0, k2 = 0, mt;

  gsl_complex i, zarr[4], w1, w2, w3;
  GSL_SET_COMPLEX(&i,     0.0, 1.0);
  GSL_SET_COMPLEX(&zarr[0], 0.0, 0.0);
  GSL_SET_COMPLEX(&zarr[1], 0.0, 0.0);
  GSL_SET_COMPLEX(&zarr[2], 0.0, 0.0);
  GSL_SET_COMPLEX(&zarr[3], 0.0, 0.0);
  GSL_SET_COMPLEX(&w1,    0.0, 0.0);
  GSL_SET_COMPLEX(&w2,    0.0, 0.0);
  GSL_SET_COMPLEX(&w3,    0.0, 0.0);

  /* Deal easily with the cases where the quartic is degenerate. The
     ordering of solutions is done explicitly. */
  if (0 == b && 0 == c) {
    if (0 == d) {
      if (a > 0) {
        GSL_SET_COMPLEX(z0, -a,  0.0);
        GSL_SET_COMPLEX(z1, 0.0, 0.0);
        GSL_SET_COMPLEX(z2, 0.0, 0.0);
        GSL_SET_COMPLEX(z3, 0.0, 0.0);
      } else {
        GSL_SET_COMPLEX(z0, 0.0, 0.0);
        GSL_SET_COMPLEX(z1, 0.0, 0.0);
        GSL_SET_COMPLEX(z2, 0.0, 0.0);
        GSL_SET_COMPLEX(z3, -a,  0.0);
      }
      return 4;
    } else if (0 == a) {
      if (d > 0) {
        *z3 = gsl_complex_sqrt(gsl_complex_mul_real(i, sqrt(d)));
        *z2 = gsl_complex_mul(gsl_complex_conjugate(i), *z3);
        *z1 = gsl_complex_negative(*z2);
        *z0 = gsl_complex_negative(*z3);
      } else {
        *z3 = gsl_complex_sqrt_real(sqrt(-d));
        *z2 = gsl_complex_mul(i, *z3);
        *z1 = gsl_complex_negative(*z2);
        *z0 = gsl_complex_negative(*z3);
      }
      return 4;
    }
  }

  if (0.0==c && 0.0==d) {
    disc = a*a-4.0*b;
    if (disc<0.0) {
      mt=3;
    } else {
      mt=1;
    }
    *z0=zarr[0];
    *z1=zarr[0];
    gsl_poly_complex_solve_quadratic(1.0,a,b,z2,z3);
  } else {
    /* For non-degenerate solutions, proceed by constructing and
       solving the resolvent cubic */
    const double aa = a * a;
    const double pp = b - q1 * aa;
    const double qq = c - q2 * a * (b - q4 * aa);
    const double rr = d - q4 * (a * c - q4 * aa * (b - q3 * aa));
    const double rc = q2 * pp;
    const double sc = q4 * (q4 * pp * pp - rr);
    const double tc = -(q8 * qq * q8 * qq);

    /* This code solves the resolvent cubic in a convenient fashion
     * for this implementation of the quartic. If there are three real
     * roots, then they are placed directly into u[].  If two are
     * complex, then the real root is put into u[0] and the real
     * and imaginary part of the complex roots are placed into
     * u[1] and u[2], respectively. Additionally, this
     * calculates the discriminant of the cubic and puts it into the
     * variable disc. */
    {
      const double qcub = (rc * rc - 3 * sc);
      const double rcub = (2 * rc * rc * rc - 9 * rc * sc + 27 * tc);

      const double Q = qcub / 9;
      const double R = rcub / 54;

      const double Q3 = Q * Q * Q;
      const double R2 = R * R;

      const double CR2 = 729 * rcub * rcub;
      const double CQ3 = 2916 * qcub * qcub * qcub;

      disc = (CR2 - CQ3) / 2125764.0;

      if (R == 0 && Q == 0)
        {
          u[0]=-rc/3;
          u[1]=-rc/3;
          u[2]=-rc/3;
        }
      else if (CR2==CQ3)
        {
          const double sqrtQ = sqrt(Q);
          if (R>0)
            {
              u[0]=-2 * sqrtQ - rc / 3;
              u[1]=sqrtQ - rc / 3;
              u[2]=sqrtQ - rc / 3;
            }
          else
            {
              u[0]= -sqrtQ - rc / 3;
              u[1]= -sqrtQ - rc / 3;
              u[2]= 2 * sqrtQ - rc / 3;
            }
        }
      else if (CR2<CQ3)
        {
          const double sqrtQ = sqrt (Q);
          const double sqrtQ3 = sqrtQ * sqrtQ * sqrtQ;
          const double theta = ( R/sqrtQ3 >= 1.0 ? 0 : acos(R / sqrtQ3) );
          const double norm = -2 * sqrtQ;

          u[0] = norm * cos (theta / 3) - rc / 3;
          u[1] = norm * cos ((theta + 2.0 * M_PI) / 3) - rc / 3;
          u[2] = norm * cos ((theta - 2.0 * M_PI) / 3) - rc / 3;
        }
      else
        {
          const double sgnR = (R >= 0 ? 1 : -1);
          const double A = -sgnR * pow (fabs (R) + sqrt (R2 - Q3), 1.0 / 3.0);
          const double B = Q / A;

          u[0] = A + B - rc / 3;
          u[1] = -0.5 * (A + B) - rc / 3;
          u[2] = -( sqrt(3.0) / 2.0) * fabs(A - B);
        }
    }
    /* End of solution to resolvent cubic */

    /* Combine the square roots of the roots of the cubic
     * resolvent appropriately. Also, calculate 'mt' which
     * designates the nature of the roots:
     * mt=1 : 4 real roots
     * mt=2 : 0 real roots
     * mt=3 : 2 real roots
     */
    if (disc==0) {
      u[2] = u[1];
    }
    if (disc <= 0) {
      mt = 2;
      v[0] = fabs(u[0]);
      v[1] = fabs(u[1]);
      v[2] = fabs(u[2]);
      double v1 = GSL_MAX(GSL_MAX(v[0], v[1]), v[2]);
      double v2;
      if (v1 == v[0]) {
        k1 = 0;
        v2 = GSL_MAX(v[1], v[2]);
      } else if (v1 == v[1]) {
        k1 = 1;
        v2 = GSL_MAX(v[0], v[2]);
      } else {
        k1 = 2;
        v2 = GSL_MAX(v[0], v[1]);
      }

      if (v2 == v[0]) {
        k2 = 0;
      } else if (v2 == v[1]) {
        k2 = 1;
      } else {
        k2 = 2;
      }
      w1 = gsl_complex_sqrt_real(u[k1]);
      w2 = gsl_complex_sqrt_real(u[k2]);
    } else {
      mt = 3;
      GSL_SET_COMPLEX(&w1, u[1], u[2]);
      GSL_SET_COMPLEX(&w2, u[1], -u[2]);
      w1=gsl_complex_sqrt(w1);
      w2=gsl_complex_sqrt(w2);
    }
    /* Solve the quadratic in order to obtain the roots
     * to the quartic */
    const double q = qq;
    if (gsl_complex_abs(gsl_complex_mul(w1, w2)) != 0.0) {
      w3 = gsl_complex_mul_real(gsl_complex_inverse(gsl_complex_mul(w1, w2)), -q / 8.0);
    }
    const double h = r4 * a;
    zarr[0] = gsl_complex_add_real(gsl_complex_add(gsl_complex_add(w1, w2), w3), -h);
    zarr[1] = gsl_complex_add_real(gsl_complex_add(gsl_complex_negative(gsl_complex_add(w1, w2)), w3), -h);
    zarr[2] = gsl_complex_add_real(gsl_complex_sub(gsl_complex_sub(w2, w1), w3), -h);
    zarr[3] = gsl_complex_add_real(gsl_complex_sub(gsl_complex_sub(w1, w2), w3), -h);

    /* Arrange the roots into the variables z0, z1, z2, z3 */
    if (2 == mt) {
      if (u[k1] >= 0 && u[k2] >= 0) {
        mt = 1;
        GSL_SET_COMPLEX(z0, GSL_REAL(zarr[0]), 0.0);
        GSL_SET_COMPLEX(z1, GSL_REAL(zarr[1]), 0.0);
        GSL_SET_COMPLEX(z2, GSL_REAL(zarr[2]), 0.0);
        GSL_SET_COMPLEX(z3, GSL_REAL(zarr[3]), 0.0);
      } else if (u[k1] >= 0 && u[k2] < 0) {
        *z0 = zarr[0];
        *z1 = zarr[3];
        *z2 = zarr[2];
        *z3 = zarr[1];
      } else if (u[k1] < 0 && u[k2] >= 0) {
        *z0 = zarr[0];
        *z1 = zarr[2];
        *z2 = zarr[3];
        *z3 = zarr[1];
      } else if (u[k1] < 0 && u[k2] < 0) {
        *z0 = zarr[0];
        *z1 = zarr[1];
        *z2 = zarr[3];
        *z3 = zarr[2];
      }
    } else if (3 == mt) {
      GSL_SET_COMPLEX(z0, GSL_REAL(zarr[0]), 0.0);
      GSL_SET_COMPLEX(z1, GSL_REAL(zarr[1]), 0.0);
      *z2 = zarr[3];
      *z3 = zarr[2];
    }
  }

  /*
   * Sort the roots as usual: main sorting by ascending real part, secondary
   * sorting by ascending imaginary part
   */

  if (1==mt) {
    /* Roots are all real, sort them by the real part */
    if (GSL_REAL(*z0) > GSL_REAL(*z1)) SWAP(*z0,*z1);
    if (GSL_REAL(*z0) > GSL_REAL(*z2)) SWAP(*z0,*z2);
    if (GSL_REAL(*z0) > GSL_REAL(*z3)) SWAP(*z0,*z3);

    if (GSL_REAL(*z1) > GSL_REAL(*z2)) SWAP(*z1,*z2);
    if (GSL_REAL(*z2) > GSL_REAL(*z3)) {
      SWAP(*z2,*z3);
      if (GSL_REAL(*z1) > GSL_REAL(*z2)) SWAP(*z1,*z2);
    }
  } else if (2==mt) {
    /* Roots are all complex. z0 and z1 are conjugates
     * and z2 and z3 are conjugates. Sort the real parts first */
    if (GSL_REAL(*z0) > GSL_REAL(*z2)) {
      SWAP(*z0,*z2);
      SWAP(*z1,*z3);
    }
    /* Then sort by the imaginary parts */
    if (GSL_IMAG(*z0)>GSL_IMAG(*z1)) SWAP(*z0,*z1);
    if (GSL_IMAG(*z2)>GSL_IMAG(*z3)) SWAP(*z2,*z3);
  } else {
    /* 2 real roots. z2 and z3 are conjugates. */

    /* Swap complex roots */
    if (GSL_IMAG(*z2)>GSL_IMAG(*z3)) SWAP(*z2,*z3);

    /* Sort real parts */
    if (GSL_REAL(*z0) > GSL_REAL(*z1)) SWAP(*z0,*z1);
    if (GSL_REAL(*z1) > GSL_REAL(*z2)) {
      if (GSL_REAL(*z0) > GSL_REAL(*z2)) {
        SWAP(*z0,*z2);
        SWAP(*z1,*z3);
      } else {
        SWAP(*z1,*z2);
        SWAP(*z2,*z3);
      }
    }

  }

  return 4;
}
