// ============================================================================
#ifndef LOKI_RELEASE_H 
#define LOKI_RELEASE_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Convertible.h"
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
 *   By usage of this code one clearly states the disagreement 
 *    with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
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
  struct Release 
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
  struct Release<TYPE,true> 
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
