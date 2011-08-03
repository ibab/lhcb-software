// ============================================================================
#ifndef DETDESC_SOLIDMATH_H 
#define DETDESC_SOLIDMATH_H 1
// Include files
#include "DetDesc/ISolid.h" 

/** @file SolidMath.h
 *  
 *  collection of mathematical utilities for dealing with SoolidTicks 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   11/05/2002
 */

namespace SolidTicks
{

  /**  Solve the quadratic equation:  a*x*x + b*x + c = 0; 
   *  @author      Vanya Belyaev   Ivan.Belyaev@itep.ru 
   *  @date        10.02.2000 
   *  @param a  equation parameter
   *  @param b  equation parameter 
   *  @param c  equation parameter 
   *  @param out output iterator 
   */
  template < class OUTPUTTYPE > 
  inline unsigned int 
  SolveQuadraticEquation
  ( const double a   , 
    const double b   , 
    const double c   ,
    OUTPUTTYPE   out ) 
  {
    if( 0 == a ) // it is indeed  a linear equation:  b*x + c = 0 
      {
        // no solution!
        if( b == 0 ) { return 0 ; }   // RETURN !!! 
        // 1 solution!
        *out++ = -1.0 * c / b ; 
        *out++ = -1.0 * c / b ;       // double the solutions 
        return 1;                     // RETURN !!!   
      }
    double d = b * b - 4.0 * a * c ; 
    // no solutions 
    if(  d < 0  )   { return 0; }     // RETURN !!!
    // 1 or 2 solution
    d = sqrt( d )                  ;   
    *out++ = 0.5 * ( -b - d ) / a  ; 
    *out++ = 0.5 * ( -b + d ) / a  ; 
    // return number of solutions;
    return 0 == d ? 1 : 2 ;           // RETURN !!! 
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
  inline unsigned int  
  LineIntersectsTheSphere
  ( const aPoint  & point  , 
    const aVector & vect   , 
    const double        radius ,
    OUTPUTTYPE          out    )
  {
    // sphere with non-positive radius is not able to intersect the line! 
    if( radius <= 0 ) { return 0 ; } 
    // line with null direction vector does not  intersect the sphere! 
    double v2 = vect.mag2(); 
    if( v2 <= 0     ) { return 0 ; }
    double p2 = point.mag2()    ; 
    double pv = point.Dot(vect) ; 
    /** It is equivalent to the equation
     *  ( Point + Vector * Tick )^2 = R^2
     *  it is quadratic equation!  a*x^2+b*x+c=0
     */
    const double a = v2                   ; 
    const double b = 2.0*pv               ; 
    const double c = p2 - radius * radius ;
    // solve the equation! 
    return SolidTicks::SolveQuadraticEquation( a , b , c , out );
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
  inline unsigned int  
  LineIntersectsTheSphere2
  ( const aPoint  & point  , 
    const aVector & vect   , 
    const double        r2     ,
    OUTPUTTYPE          out    )
  {
    // sphere with non-positive radius is not able to intersect the line! 
    if( r2 <= 0     ) { return 0 ; } 
    // line with null direction vector does not  intersect the sphere! 
    double v2 = vect.mag2();
    if( v2 <= 0     ) { return 0 ; }
    double p2 = point.mag2()    ; 
    double pv = point.Dot(vect) ; 
    /** It is equivalent to the equation
     *  ( Point + Vector * Tick )^2 = R^2
     *  it is quadratic equation!  a*x^2+b*x+c=0
     */
    const double a = v2      ; 
    const double b = 2.0*pv  ; 
    const double c = p2 - r2 ;
    // solve the equation! 
    return SolidTicks::SolveQuadraticEquation( a , b , c , out );
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
  inline unsigned int  
  LineIntersectsTheCylinder
  ( const aPoint  & point  , 
    const aVector & vect   , 
    const double        radius ,
    OUTPUTTYPE          out    )
    
  {
    // Cylinder with non-positive radius is not able to intersect the line! 
    if( radius <= 0 ) { return 0 ; }     
    // line with null direction vector is not able to intersect the cylinder! 
    const double v2 = vect.x()*vect.x() + vect.y()*vect.y() ; 
    if( v2 <= 0     ) { return 0 ; }
    //
    const double p2 = point.x() * point.x() + point.y() * point.y() ; 
    const double pv = point.x() * vect.x()  + point.y() * vect.y()  ; 
    /** It is equivalent to the equation
     *	( Point + Vector * Tick )^2 = R^2
     *  it is quadratic equation!  a*x^2+b*x+c=0
     */
    const double a = v2                   ; 
    const double b = 2.0*pv               ; 
    const double c = p2 - radius * radius ;
    // solve the equation! 
    return SolidTicks::SolveQuadraticEquation( a , b , c , out );  
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
  inline unsigned int  
  LineIntersectsTheX
  ( const aPoint  & point  , 
    const aVector & vect   , 
    const double        X      ,
    OUTPUTTYPE          out   )
    
  {
    /**  line with null vector in X-direction is not 
     *    able to intersect the x-plane! 
     */
    if( 0 == vect.x() ) { return 0; } 
    *out++ = ( X - point.x() ) / vect.x() ; 
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
  inline unsigned int  
  LineIntersectsTheY
  ( const aPoint  & point  , 
    const aVector & vect   , 
    const double        Y      ,
    OUTPUTTYPE          out    )
    
  {    
    /**  line with null vector in Y-direction is not able 
     *   to intersect the y-plane! 
     */
    if( 0 == vect.y() ) { return 0; } 
    *out++ = ( Y - point.y() ) / vect.y() ; 
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
  inline unsigned int  
  LineIntersectsTheZ
  ( const aPoint & point  , 
    const aVector& vect   , 
    const double       Z      ,
    OUTPUTTYPE         out    )
    
  {    
    /**  line with null vector in Z-direction is 
     *  not able to intersect the z-plane! 
     */
    if( 0 == vect.z() ) { return 0; } 
    *out++ = ( Z - point.z() ) / vect.z() ; 
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
  inline unsigned int  
  LineIntersectsThePhi
  ( const aPoint  & point  , 
    const aVector & vect   , 
    const double        Phi    ,
    OUTPUTTYPE          out    )
    
  {
    const double sinphi = sin( Phi ) ; 
    const double cosphi = cos( Phi ) ; 
    const double d      = vect.x() * sinphi - vect.y() * cosphi ; 
    if( 0 == d ) { return 0; } 
    // only accept half the phi plane !
    const double e      = vect.y() * point.x() - vect.x() * point.y() ;
    if( e * d > 0 ) { return 0 ; }
    *out++ = ( point.y() * cosphi - point.x() * sinphi ) / d ; 
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
  inline unsigned int  
  LineIntersectsTheTheta
  ( const aPoint & point  , 
    const aVector & vect   , 
    const double       Theta  ,
    OUTPUTTYPE         out    )
    
  { 
    /** it is equivalent to solve the equation
     *	cos^2(x^2+y^2)=sin^2*z^2
     */
    const double sinthe = sin( Theta )    ; 
    const double costhe = cos( Theta )    ;
    
    const double c2     = costhe * costhe ; 
    const double s2     = sinthe * sinthe ; 
    
    const double a = 
      c2 *  vect.x() *  vect.x() + 
      c2 *  vect.y() *  vect.y() - 
      s2 *  vect.z() *  vect.z() ; 
    double       b = 
      c2 *  vect.x() * point.x() + 
      c2 *  vect.y() * point.y() - 
      s2 *  vect.z() * point.z() ;
    const double c = 
      c2 * point.x() * point.x() + 
      c2 * point.y() * point.y() - 
      s2 * point.z() * point.z() ;
    
    b *= 2.0;

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
  inline unsigned int  
  LineIntersectsTheCone
  ( const aPoint  & point  , 
    const aVector & vect   , 
    const double        r1     ,
    const double        r2     ,
    const double        z1     ,
    const double        z2     ,
    OUTPUTTYPE          out    )
    
  {
    /** it is equivalent to the equation
     *  (x^2+y^2)=(r(z))^2
     *  
     *  r(z) = r1 + (z-z1)*(r2-r1)/(z2-z1) = 
     *  = r1 + (z-z1)*(drdz)  = 
     *  = r1 - z1*drdz + drdz*(pz+vz*t) = 
     *  = r1 - z1*drdz + drdz*pz + drdz*vz*t = 
     *  = p1 + p2*t ; 
     */
    const double drdz = (r2-r1)/(z2-z1)               ;
    const double p1   = r1 - z1*drdz + drdz*point.z() ;
    const double p2   = drdz * vect.z()               ;
    
    double a = vect.x () * vect.x () + vect.y () * vect.y () ;
    a -= p2*p2 ; 
    double b = vect.x () * point.x() + vect.y () * point.y() ;
    b -= p2*p1 ;
    b *= 2.0   ; 
    double c = point.x() * point.x() + point.y() * point.y() ; 
    c -= p1*p1 ;
    
    // return result
    return SolidTicks::SolveQuadraticEquation( a , b, c, out );
  }

} // end of namespace 

// ============================================================================
// The END
// ============================================================================
#endif // DETDESC_SOLIDMATH_H
// ============================================================================
