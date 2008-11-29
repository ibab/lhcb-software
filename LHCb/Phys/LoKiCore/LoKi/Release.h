// $Id: Release.h,v 1.6 2008-11-29 13:24:59 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_RELEASE_H 
#define LOKI_RELEASE_H 1
// ============================================================================
// Include files
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Convertible.h"
#include "LoKi/AddRef.h"
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
 *  @date 2006-01-23 
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @struct Release Release.h LoKi/Release.h
   * 
   *  Helper utility to resolve the templated 
   *  ambiguity for GCC compiler 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-11
   */
  template <class TYPE,bool>
  struct Release : public std::unary_function<TYPE*,bool>
  {
    // ========================================================================
    bool operator() ( TYPE*       o )
    { 
      if ( 0 != o ) { o->release() ; } 
      return 0 != o ;
    }
    // ========================================================================
  } ;
  // ==========================================================================
  template <class TYPE>
  struct Release<TYPE,true> : public std::unary_function<IInterface*,bool>
  {
    // ========================================================================
    bool operator() ( IInterface* o ) 
    {
      if ( 0 != o ) { o->release() ; } 
      return 0 != o ;
    }
    // =======================================================================
  } ;
  // ==========================================================================  
  /** helper function to invoke properly TYPE::release method 
   *  avoiding the template ambiguity for GCC compiler 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-11
   */
  template <class TYPE>
  inline bool release( TYPE* obj ) 
  {
    // ========================================================================
    typedef LoKi::Convertible<TYPE,IInterface> _Tester ;
    // create the proper actor
    Release<TYPE,_Tester::value> actor ;
    // and perform the proper action!
    return actor( obj );
    // ========================================================================
  } 
  // ==========================================================================
} //end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_RELEASE_H
// ============================================================================
