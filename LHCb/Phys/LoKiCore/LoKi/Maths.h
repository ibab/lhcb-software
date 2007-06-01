// $Id: Maths.h,v 1.1 2007-06-01 11:35:26 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_MATHS_H 
#define LOKI_MATHS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/Math.h"
#include "LoKi/Operators.h"
// ============================================================================
namespace LoKi 
{
  namespace Dicts
  {
    /** @class Maths Maths.h LoKi/Maths.h
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-29
     */
    template <class TYPE>
    class Maths
    {
    private:
      typedef LoKi::Function<TYPE>             Func ;    
      typedef LoKi::FunctionFromFunction<TYPE> Fun  ;    
    public:
      static Fun   exp   ( const Func&  f  ) { return LoKi::  exp   ( f ) ; }
      
      static Fun   sin   ( const Func&  f  ) { return LoKi::  sin   ( f ) ; }
      static Fun   sinh  ( const Func&  f  ) { return LoKi::  sinh  ( f ) ; }
      static Fun  asin   ( const Func&  f  ) { return LoKi:: asin   ( f ) ; }
      
      static Fun   cos   ( const Func&  f  ) { return LoKi::  cos   ( f ) ; }
      static Fun   cosh  ( const Func&  f  ) { return LoKi::  cosh  ( f ) ; }
      static Fun  acos   ( const Func&  f  ) { return LoKi:: acos   ( f ) ; }
      
      static Fun   tan   ( const Func&  f  ) { return LoKi::  tan   ( f ) ; }
      static Fun   tanh  ( const Func&  f  ) { return LoKi::  tanh  ( f ) ; }
      static Fun  atan   ( const Func&  f  ) { return LoKi:: atan   ( f ) ; }
      
      static Fun   sqrt  ( const Func&  f  ) { return LoKi::  sqrt  ( f ) ; }
      static Fun   log   ( const Func&  f  ) { return LoKi::  log   ( f ) ; }
      static Fun   log10 ( const Func&  f  ) { return LoKi::  log10 ( f ) ; }
      static Fun   fabs  ( const Func&  f  ) { return LoKi::  fabs  ( f ) ; }
      
      static Fun   atan2 ( const Func&  f1 , 
                           const Func&  f2 ) { return LoKi::atan2 ( f1 , f2 ) ; }
      static Fun   atan2 ( const Func&  f1 , 
                           const double f2 ) { return LoKi::atan2 ( f1 , f2 ) ; }
      static Fun   atan2 ( const double f1 , 
                           const Func&  f2 ) { return LoKi::atan2 ( f1 , f2 ) ; }
      
      static Fun   pow2  ( const Func&  f  ) { return LoKi::pow2  ( f ) ; }
      static Fun   pow3  ( const Func&  f  ) { return LoKi::pow3  ( f ) ; }
      static Fun   pow4  ( const Func&  f  ) { return LoKi::pow4  ( f ) ; }
      //
    } ;
  } // end of namespace LoKi::Dicts
} // end of namespace LoKi
// ============================================================================
/// The END 
// ============================================================================
#endif // LOKI_MATHS_H
// ============================================================================
