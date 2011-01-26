// $Id: IMCHybridTool.h,v 1.2 2008-02-19 15:04:01 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_IMCHYBRIDTOOL_H 
#define LOKI_IMCHYBRIDTOOL_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <string>
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/MCTypes.h"
// ============================================================================
namespace LoKi
{ 
  // ==========================================================================
  /** @class IMCHybridTool IMCHybridTool.h LoKi/IMCHybridTool.h
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
  class GAUDI_API IMCHybridTool : public virtual IAlgTool 
  {
  public:
    // ========================================================================
    /// Return the unique interface ID
    static const InterfaceID& interfaceID() ;
    // ========================================================================
  public:
    // ========================================================================
    // predicates:
    // ========================================================================
    /// set the C++ predicate for LHCb::MCParticle
    virtual void set ( const LoKi::Types::MCCuts&  cut ) = 0 ;    
    /// set the C++ predicate for LHCb::MCVertex 
    virtual void set ( const LoKi::Types::MCVCuts& cut ) = 0 ;    
    // ========================================================================
  public:
    // ========================================================================
    // functions 
    // ========================================================================
    /// set the C++ function for LHCb::MCParticle 
    virtual void set ( const LoKi::Types::MCFunc&  cut ) = 0 ;
    /// set the C++ function for LHCb::MCVertex 
    virtual void set ( const LoKi::Types::MCVFunc& cut ) = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    // maps:
    // ========================================================================
    /// set the C++ map for LHCb::MCParticle 
    virtual void set ( const LoKi::Types::MCMaps&  cut ) = 0 ;
    /// set the C++ map for LHCb::MCVertex 
    virtual void set ( const LoKi::Types::MCVMaps& cut ) = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    // pipes:
    // ========================================================================
    /// set the C++ pipe for LHCb::MCParticle 
    virtual void set ( const LoKi::Types::MCPipes&  cut ) = 0 ;
    /// set the C++ pipe for LHCb::MCVertex 
    virtual void set ( const LoKi::Types::MCVPipes& cut ) = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    // fun-vals:
    // ========================================================================
    /// set the C++ fun-val for LHCb::MCParticle 
    virtual void set ( const LoKi::Types::MCFunVals&  cut ) = 0 ;
    /// set the C++ fun-val for LHCb::MCVertex 
    virtual void set ( const LoKi::Types::MCVFunVals& cut ) = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    // elements
    // ========================================================================
    /// set the C++ element for LHCb::MCParticle 
    virtual void set ( const LoKi::Types::MCElements&  cut ) = 0 ;
    /// set the C++ element for LHCb::MCVertex 
    virtual void set ( const LoKi::Types::MCVElements& cut ) = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    // sources
    // ========================================================================
    /// set the C++ source for LHCb::MCParticle 
    virtual void set ( const LoKi::Types::MCSources&  cut ) = 0 ;
    /// set the C++ source for LHCb::MCVertex 
    virtual void set ( const LoKi::Types::MCVSources& cut ) = 0 ;
    // ========================================================================
  protected:
    // ========================================================================
    // destructor : virtual and protected 
    virtual ~IMCHybridTool() ; ///< destructor : virtual and protected 
    // ========================================================================
  };
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IMCHYBRIDTOOL_H
// ============================================================================
