// $Id: cpower.h,v 1.3 2006-05-02 14:29:10 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_CPOWER_H 
#define LOKI_CPOWER_H 1
// ============================================================================
// Include files
// ============================================================================

// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

namespace LoKi 
{  
  
  /** @fn cpower 
   *  simple utility for efficient "pow"
   *
   *  @code 
   *   
   *   const double result = LoKi::cpower ( value , 10 ) ;
   *
   *  @code 
   *
   *  The actual code is copied from 
   *     std::__cmath_power bits/cmath.tcc 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
   *  @date 2005-04-09 
   */
  template<typename TYPE>
  inline TYPE cpower ( TYPE __x , unsigned long __n )
  {
    TYPE __y = __n % 2 ? __x : 1;
    
    while ( __n >>= 1 )
    {
      __x = __x * __x;
      if ( __n % 2) { __y = __y * __x; }
    } ;
    
    return __y ;
  };
  
}; // end of namespace LoKi 

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CPOWER_H
// ============================================================================
