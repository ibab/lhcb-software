// $Id: MathHelperFun.h,v 1.1 2006-05-31 13:38:31 jpalac Exp $
#ifndef KERNEL_MATHHELPERFUN_H 
#define KERNEL_MATHHELPERFUN_H 1

// Include files

/** @file MathHelperFun.h Kernel/MathHelperFun.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2006-05-02
 */
namespace Gaudi {
  namespace Math 
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
    SolveQuadraticEquation( const double a, 
                            const double b, 
                            const double c,
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
    };


  }; // Math namespace
   
}; // Gaudi namespace
#endif // KERNEL_MATHHELPERFUN_H
