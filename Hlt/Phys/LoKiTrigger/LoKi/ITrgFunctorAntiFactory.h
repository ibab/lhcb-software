// $Id$
// ============================================================================
#ifndef LOKI_ITRGANTIFACTORY_H 
#define LOKI_ITRGANTIFACTORY_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/TriggerTypes.h"
// ============================================================================
namespace LoKi
{ 
  // ==========================================================================
  /** @class ITrgFunctorAntiFactory LoKi/ITrgfunctorAntiFactory
   *
   *  Helper interface for implementation of C++/Python "Hybrid" solution
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
   *  @date   2004-06-29
   *
   *  $Revision$
   *  Last Modification $Date$ 
   *                 by $Author$ 
   *
   */
  class ITrgFunctorAntiFactory 
    : public virtual extend_interfaces<IAlgTool>
  {
  public:
    // ========================================================================
    /// set the C++ predicate for Hlt::Candidate
    virtual void set ( const LoKi::Types::TC_Cuts&    cut ) = 0 ;    
    /// set the C++ function for Hlt::Candidate
    virtual void set ( const LoKi::Types::TC_Func&    fun ) = 0 ;    
    // ========================================================================
  public:
    // ========================================================================
    // the functional part
    /// set the C++ "maps"     for Candidates 
    virtual void set ( const LoKi::Types::TC_Maps&     fun ) = 0 ;
    /// set the C++ "pipes"    for Candidates 
    virtual void set ( const LoKi::Types::TC_Pipes&    fun ) = 0 ;
    /// set the C++ "funvals"  for Candidates 
    virtual void set ( const LoKi::Types::TC_FunVals&  fun ) = 0 ;
    /// set the C++ "cutvals"  for Candidates 
    virtual void set ( const LoKi::Types::TC_CutVals&  fun ) = 0 ;
    /// set the C++ "sources"  for Candidates 
    virtual void set ( const LoKi::Types::TC_Sources&  fun ) = 0 ;
    // ========================================================================
  protected:
    // ========================================================================
    /// destructor : virtual and protected 
    virtual ~ITrgFunctorAntiFactory() ;   // destructor : virtual and protected 
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_ITRHYBRIDTOOL_H
// ============================================================================
