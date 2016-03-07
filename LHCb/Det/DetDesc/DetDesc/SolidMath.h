// ============================================================================
#ifndef DETDESC_SOLIDMATH_H
#define DETDESC_SOLIDMATH_H 1
// Include files
#include <cmath> // std::sqrt, std::sin, std::cos, std::fma, std::copysign
#include <algorithm> // std::min, std::max
#include "GaudiKernel/Kernel.h" // (UN)LIKELY

#include "DetDesc/ISolid.h"

/** @file SolidMath.h
 *
 *  collection of mathematical utilities for dealing with SoolidTicks
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   11/05/2002
 */

namespace SolidTicks
{
  /** Solve the quadratic equation:  a*x*x + b*x + c = 0;
   *
   * @author      Vanya Belyaev   Ivan.Belyaev@itep.ru
   * @date        10.02.2000
   *
   * @param a  equation parameter
   * @param b  equation parameter
   * @param c  equation parameter
   * @param out output iterator
   *
   * @returns number of solutions
   *
   * @author Manuel Schiller	Manuel.Schiller@cern.ch
   * @date 2015-02-20
   * - beefed up version of the routine which tries quite hard to avoid
   *   numerical trouble
   */
  template < class OUTPUTTYPE >
  inline unsigned int SolveQuadraticEquation(double a, double b, double c,
      OUTPUTTYPE out) noexcept(noexcept(*out++))
  {
    if (UNLIKELY(0 == a)) { // it is indeed  a linear equation:  b*x + c = 0
      if (UNLIKELY(0 == b)) return 0;   // RETURN !!!  1 solution!
      c /= -b;
      *out++ = c;
      *out++ = c;           // double the solutions
      return 1;             // RETURN !!!
    } else if (UNLIKELY(0 == b)) { // simple case: a x^2 + c = 0
      c /= a;
      if (UNLIKELY(0 < c)) return 0; // RETURN !!! (no solutions)
      c = std::sqrt(-c);
      *out++ = -c;
      *out++ = +c;
      return 0 == c ? 1 : 2; // RETURN !!!
    }
    // b^2 - c, avoid catastrophic cancellation cases with fma if we have fast
    // fma, or long double if we don't have it
#if defined(FP_FAST_FMA)
    double d = std::fma(b, b, -4 * a * c);
#else
    long double dtmp = b; dtmp *= dtmp; dtmp += -4 * a * c;
    double d = dtmp;
#endif
    if (UNLIKELY(0 > d)) return 0; // RETURN !!! (no solutions)
    // 1 or 2 solutions
    d = sqrt(d);
    // avoid loss of significance
    d = -0.5 * (b + std::copysign(d, b));
    const double x1 = d / a;
    const double x2 = c / d;
    *out++ = std::min(x1, x2); // smaller root is first
    *out++ = std::max(x1, x2);
    // return number of solutions;
    return 0 == d ? 1 : 2;           // RETURN !!!
  }

  /** find intersection ticks for the line parametrized as
   *  Point + Vector * Tick  with sphere of radius Radius
   *  @author      Vanya Belyaev   Ivan.Belyaev@itep.ru
   *  @date        10.02.2000
   *  @param point  point for line parametrisation
   *  @param vect   vector along the line
   *  @param r2     sphere radius squared
   *  @param out     output iterator
   *  @return number of intersections
   */
  template < class OUTPUTTYPE, class aPoint, class aVector >
  inline unsigned int LineIntersectsTheSphere2(const aPoint& point,
      const aVector& vect, const double r2, OUTPUTTYPE out) noexcept(noexcept(*out++))
  {
    // sphere with non-positive radius is not able to intersect the line!
    if( r2 <= 0     ) { return 0; }
    // line with null direction vector does not  intersect the sphere!
    double v2 = vect.mag2();
    if( v2 <= 0     ) { return 0; }
    double p2 = point.mag2();
    double pv = point.Dot(vect);
    /** It is equivalent to the equation
     *  ( Point + Vector * Tick )^2 = R^2
     *  it is quadratic equation!  a*x^2+b*x+c=0
     */
    // solve the equation!
    return SolidTicks::SolveQuadraticEquation( v2 , 2 * pv, p2 - r2 , out );
  }

  /** find intersection ticks for the line parametrized as
   *  Point + Vector * Tick  with sphere of radius Radius
   *  @author      Vanya Belyaev   Ivan.Belyaev@itep.ru
   *  @date        10.02.2000
   *  @param point  point for line parametrisation
   *  @param vect   vector along the line
   *  @param radius  sphere radius
   *  @param out     output iterator
   *  @return number of intersections
   */
  template < class OUTPUTTYPE, class aPoint, class aVector >
  inline unsigned int LineIntersectsTheSphere(const aPoint& point,
      const aVector& vect, const double radius, OUTPUTTYPE out) noexcept(noexcept(*out++))
  {
    // reuse!
    return LineIntersectsTheSphere2(point, vect, radius * std::abs(radius), out);
  }

  /** find intersection ticks for the line parametrized
   *  as Point + Vector * Tick with cylinder of radius Radius
   *  @author      Vanya Belyaev   Ivan.Belyaev@itep.ru
   *  @date        10.02.2000
   *  @param point  point for line parametrisation
   *  @param vect    vector along the line
   *  @param radius  cylinder radius
   *  @param out     output iterator
   *  @return number of intersections
   */
  template < class OUTPUTTYPE, class aPoint, class aVector >
  inline unsigned int LineIntersectsTheCylinder(const aPoint& point,
      const aVector& vect, const double radius, OUTPUTTYPE out) noexcept(noexcept(*out++))
  {
    // Cylinder with non-positive radius is not able to intersect the line!
    if( radius <= 0 ) { return 0; }
    // line with null direction vector is not able to intersect the cylinder!
    const double v2 = vect.x()*vect.x() + vect.y()*vect.y();
    if( v2 <= 0     ) { return 0; }
    //
    const double p2 = point.x() * point.x() + point.y() * point.y();
    const double pv = point.x() * vect.x()  + point.y() * vect.y();
    /** It is equivalent to the equation
     *	( Point + Vector * Tick )^2 = R^2
     *  it is quadratic equation!  a*x^2+b*x+c=0
     */
    // solve the equation!
    return SolidTicks::SolveQuadraticEquation( v2, 2 * pv, p2 - radius * radius , out );
  }

  /** find intersection ticks for the line parametrized
   *  as Point + Vector * Tick with x-plane x=X
   *  @author      Vanya Belyaev   Ivan.Belyaev@itep.ru
   *  @date        10.02.2000
   *  @param point  point for line parametrisation
   *  @param vect    vector along the line
   *  @param X       x-parameter
   *  @param out     output iterator
   *  @return number of intersections
   */
  template < class OUTPUTTYPE, class aPoint, class aVector >
  inline unsigned int LineIntersectsTheX(const aPoint& point,
      const aVector& vect, const double X, OUTPUTTYPE out) noexcept(noexcept(*out++))
  {
    /**  line with null vector in X-direction is not
     *    able to intersect the x-plane!
     */
    if( 0 == vect.x() ) { return 0; }
    *out++ = ( X - point.x() ) / vect.x();
    return 1;
  }

  /** find intersection ticks for the line
   *  parametrized as Point + Vector * Tick with y-plane y=Y
   *  @author      Vanya Belyaev   Ivan.Belyaev@itep.ru
   *  @date        10.02.2000
   *  @param  point   point for line parametrisation
   *  @param  vect    vector along the line
   *  @param  Y       y-parameter
   *  @param  out     output iterator
   *  @return number  of intersections
   */
  template < class OUTPUTTYPE, class aPoint, class aVector >
  inline unsigned int LineIntersectsTheY(const aPoint& point,
      const aVector& vect, const double Y, OUTPUTTYPE out) noexcept(noexcept(*out++))
  {
    /**  line with null vector in Y-direction is not able
     *   to intersect the y-plane!
     */
    if( 0 == vect.y() ) { return 0; }
    *out++ = ( Y - point.y() ) / vect.y();
    return 1;
  }

  /** find intersection ticks for the line
   *  parametrized as Point + Vector * Tick with z-plane z=Z
   *  @author      Vanya Belyaev   Ivan.Belyaev@itep.ru
   *  @date        10.02.2000
   *  @param  point   point for line parametrisation
   *  @param  vect    vector along the line
   *  @param  Z       z-parameter
   *  @param  out     output iterator
   *  @return number  of intersections
   */
  template < class OUTPUTTYPE, class aPoint, class aVector >
  inline unsigned int LineIntersectsTheZ(const aPoint& point,
      const aVector& vect, const double Z, OUTPUTTYPE out) noexcept(noexcept(*out++))
  {
    /**  line with null vector in Z-direction is
     *  not able to intersect the z-plane!
     */
    if( 0 == vect.z() ) { return 0; }
    *out++ = ( Z - point.z() ) / vect.z();
    return 1;
  }

  /** find intersection ticks for the line parametrized
   *  as Point + Vector * Tick  with half-plane phi=Phi
   *  @author      Vanya Belyaev   Ivan.Belyaev@itep.ru
   *  @date        10.02.2000
   *  @param  point   point for line parametrisation
   *  @param  vect    vector along the line
   *  @param  Phi     phi-parameter
   *  @param  out     output iterator
   *  @return number  of intersections
   */
  template < class OUTPUTTYPE, class aPoint, class aVector >
  inline unsigned int LineIntersectsThePhi(const aPoint& point,
      const aVector& vect, const double Phi, OUTPUTTYPE out) noexcept(noexcept(*out++))
  {
    const double sinphi = std::sin( Phi );
    const double cosphi = std::cos( Phi );
    const double d      = vect.x() * sinphi - vect.y() * cosphi;
    if( 0 == d ) { return 0; }
    // only accept half the phi plane !
    const double e      = vect.y() * point.x() - vect.x() * point.y();
    if( e * d > 0 ) { return 0; }
    *out++ = ( point.y() * cosphi - point.x() * sinphi ) / d;
    return 1;
  }

  /** find intersection ticks for the line parametrized
   *  as Point + Vector * Tick  with cone theta=Theta
   *  @author      Vanya Belyaev   Ivan.Belyaev@itep.ru
   *  @date        10.02.2000
   *  @param  point   point for line parametrisation
   *  @param  vect    vector along the line
   *  @param  Theta     Theta-parameter
   *  @param  out     output iterator
   *  @return number  of intersections
   */
  template < class OUTPUTTYPE, class aPoint, class aVector >
  inline unsigned int LineIntersectsTheTheta(const aPoint & point,
    const aVector& vect, const double Theta, OUTPUTTYPE out) noexcept(noexcept(*out++))
  {
    /** it is equivalent to solve the equation
     *	cos^2(x^2+y^2)=sin^2*z^2
     */
    const double sinthe = std::sin( Theta );
    const double costhe = std::cos( Theta );

    const double c2     = costhe * costhe;
    const double s2     = sinthe * sinthe;

    const double a =
      c2 * (vect.x() *  vect.x() + vect.y() *  vect.y()) -
      s2 *  vect.z() *  vect.z();
    const double b = 2 *
      (c2 * (vect.x() * point.x() + vect.y() * point.y()) -
       s2 *  vect.z() * point.z());
    const double c =
      c2 * (point.x() * point.x() + point.y() * point.y()) -
      s2 * point.z() * point.z();

    // return the result
    return SolidTicks::SolveQuadraticEquation( a , b, c, out );
  }


  /** find intersection ticks for the line parametrized
   *  as Point + Vector * Tick with conical surface
   *  @author      Vanya Belyaev   Ivan.Belyaev@itep.ru
   *  @date        10.02.2000
   *
   *
   *  @param  point   point for line parametrisation
   *  @param  vect    vector along the line
   *  @param  r1      cone-parameter
   *  @param  r2      cone-parameter
   *  @param  z1      cone-parameter
   *  @param  z2      cone-parameter
   *  @param  out     output iterator
   *  @return number  of intersections
   */
  template < class OUTPUTTYPE, class aPoint, class aVector >
  inline unsigned int LineIntersectsTheCone(const aPoint& point,
      const aVector& vect, const double r1, const double r2,
      const double z1, const double z2, OUTPUTTYPE out) noexcept(noexcept(*out++))
  {
    /** it is equivalent to the equation
     *  (x^2+y^2)=(r(z))^2
     *
     *  r(z) = r1 + (z-z1)*(r2-r1)/(z2-z1) =
     *  = r1 + (z-z1)*(drdz)  =
     *  = r1 - z1*drdz + drdz*(pz+vz*t) =
     *  = r1 - z1*drdz + drdz*pz + drdz*vz*t =
     *  = p1 + p2*t;
     */
    const double drdz = (r2-r1)/(z2-z1);
    const double p1   = r1 + drdz*(point.z() - z1);
    const double p2   = drdz * vect.z();

    const double a =  vect.x () * vect.x () + vect.y () * vect.y () - p2 * p2;
    const double b = (vect.x () * point.x() + vect.y () * point.y() - p2 * p1) * 2;
    const double c =  point.x() * point.x() + point.y() * point.y() - p1 * p1;

    // return result
    return SolidTicks::SolveQuadraticEquation( a , b, c, out );
  }

} // end of namespace

// ============================================================================
// The END
// ============================================================================
#endif // DETDESC_SOLIDMATH_H
// ============================================================================
