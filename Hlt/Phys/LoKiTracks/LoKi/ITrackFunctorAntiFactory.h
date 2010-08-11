// $Id$
// ============================================================================
#ifndef LOKI_ITRACKHYBRIDTOOL_H 
#define LOKI_ITRACKHYBRIDTOOL_H 1
// ============================================================================
// $URL$
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
#include "LoKi/TrackTypes.h"
// ============================================================================
namespace LoKi
{ 
  // ==========================================================================
  /** @class ITrackFunctorAntiFactory LoKi/ITrackfunctorAntiFactory
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
  class ITrackFunctorAntiFactory : public virtual IAlgTool 
  {
  public:
    // ========================================================================
    /// InterfaceID
    DeclareInterfaceID ( ITrackFunctorAntiFactory , 1 , 0 ) ;
    // ========================================================================
  public:
    // ========================================================================
    /// set the C++ predicate for LHCb::Track
    virtual void set ( const LoKi::Types::TrCuts&    cut ) = 0 ;    
    /// set the C++ function for LHCb::Track
    virtual void set ( const LoKi::Types::TrFunc&    fun ) = 0 ;    
    // ========================================================================
  public:
    // ========================================================================
    // the functional part
    /// set the C++ "maps"     for Tracks 
    virtual void set ( const LoKi::Types::TrMaps&     fun ) = 0 ;
    /// set the C++ "pipes"    for Tracks 
    virtual void set ( const LoKi::Types::TrPipes&    fun ) = 0 ;
    /// set the C++ "funvals"  for Tracks 
    virtual void set ( const LoKi::Types::TrFunVals&  fun ) = 0 ;
    /// set the C++ "cutvals"  for Tracks 
    virtual void set ( const LoKi::Types::TrCutVals&  fun ) = 0 ;
    /// set the C++ "elements" for Tracks 
    virtual void set ( const LoKi::Types::TrElements& fun ) = 0 ;
    /// set the C++ "sources"  for Tracks 
    virtual void set ( const LoKi::Types::TrSources&  fun ) = 0 ;
    // ========================================================================
  protected:
    // ========================================================================
    /// destructor : virtual and protected 
    virtual ~ITrackFunctorAntiFactory() ; // destructor : virtual and protected 
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_ITRHYBRIDTOOL_H
// ============================================================================
