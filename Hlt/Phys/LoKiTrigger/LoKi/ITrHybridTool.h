// $Id: ITrHybridTool.h,v 1.2 2007-12-11 18:56:21 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_ITRHYBRIDTOOL_H 
#define LOKI_ITRHYBRIDTOOL_H 1
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
  /** @class ITrHybridTool LoKi/ITrHybridTool.h
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
   */
  class ITrHybridTool : public virtual IAlgTool 
  {
  public:
    // ========================================================================
    /// Return the unique interface ID
    static const InterfaceID& interfaceID() ;
    // ========================================================================
  public:
    // ========================================================================
    /// set the C++ predicate for LHCb::Track
    virtual void set ( const LoKi::Types::TrCuts&    cut ) = 0 ;    
    /// set the C++ predicate for LHCb::RecVertex 
    virtual void set ( const LoKi::Types::RVCuts&    cut ) = 0 ;    
    /// set the C++ predicate for Track-Track pair 
    virtual void set ( const LoKi::Types::TTrCuts&   cut ) = 0 ;    
    /// set the C++ predicate for Track-Vertex pair
    virtual void set ( const LoKi::Types::TrVCuts&   cut ) = 0 ;    
    /// set the C++ predicate for Vertex-Vertex pair
    virtual void set ( const LoKi::Types::RVVCuts&   cut ) = 0 ;    
    /// set the C++ function for LHCb::Track
    virtual void set ( const LoKi::Types::TrFunc&    fun ) = 0 ;    
    /// set the C++ function for LHCb::RecVertex 
    virtual void set ( const LoKi::Types::RVFunc&    fun ) = 0 ;    
    /// set the C++ function for Track-Track pair 
    virtual void set ( const LoKi::Types::TTrFunc&   fun ) = 0 ;    
    /// set the C++ function for Track-Vertex pair
    virtual void set ( const LoKi::Types::TrVFunc&   fun ) = 0 ;    
    /// set the C++ function for Vertex-Vertex pair
    virtual void set ( const LoKi::Types::RVVFunc&   fun ) = 0 ;
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
    /// set the C++ "elements" for Tracks 
    virtual void set ( const LoKi::Types::TrElements& fun ) = 0 ;
    /// set the C++ "sources"  for Tracks 
    virtual void set ( const LoKi::Types::TrSources&  fun ) = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    /// get the reference container of tracks   (needed from Trigger) 
    virtual std::vector<LHCb::Track*>*     tracks   () const = 0 ;
    /// get the reference container of vertices (needed for Trigger) 
    virtual std::vector<LHCb::RecVertex*>* vertices () const = 0 ;
    // ========================================================================
  protected:
    // ========================================================================
    /// destructor : virtual and protected 
    virtual ~ITrHybridTool() ;
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_ITRHYBRIDTOOL_H
// ============================================================================
