// $Id: IHybridTool.h,v 1.4 2009-11-17 12:41:41 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_IHYBRIDTOOL_H 
#define LOKI_IHYBRIDTOOL_H 1
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
#include "LoKi/PhysTypes.h"
#include "LoKi/ATypes.h"
// ============================================================================
namespace LoKi
{ 
  // ==========================================================================
  /** @class IHybridTool IHybridTool.h LoKi/IHybridTool.h
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
  class IHybridTool : public virtual IAlgTool 
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
    /// set the C++ predicate for LHCb::Particle
    virtual void set ( const LoKi::Types::Cuts&    cut ) = 0 ;    
    /// set the C++ predicate for LHCb::Vertex 
    virtual void set ( const LoKi::Types::VCuts&   cut ) = 0 ;    
    /// set the C++ predicate for arary of particles 
    virtual void set ( const LoKi::Types::ACuts&   cut ) = 0 ;    
    // ========================================================================
    // functions:
    // ========================================================================
    /// set the C++ function for LHCb::Particle  
    virtual void set ( const LoKi::Types::Func&    cut ) = 0 ;
    /// set the C++ function for LHCb::Vertex 
    virtual void set ( const LoKi::Types::VFunc&   cut ) = 0 ;
    /// set the C++ function for arary of particles 
    virtual void set ( const LoKi::Types::AFunc&   cut ) = 0 ;    
    // ========================================================================
    // maps:
    // ========================================================================
    /// set the C++ "map" for LHCb::Particle  
    virtual void set ( const LoKi::Types::Maps&    cut ) = 0 ;
    /// set the C++ "map" for LHCb::Vertex 
    virtual void set ( const LoKi::Types::VMaps&   cut ) = 0 ;
    // ========================================================================
    // pipes: 
    // ========================================================================
    /// set the C++ "pipe" for LHCb::Particle  
    virtual void set ( const LoKi::Types::Pipes&   cut ) = 0 ;
    /// set the C++ "pipe" for LHCb::Vertex 
    virtual void set ( const LoKi::Types::VPipes&  cut ) = 0 ;
    // ========================================================================
    // fun-vals:
    // ========================================================================
    /// set the C++ "fun-val" for LHCb::Particle  
    virtual void set ( const LoKi::Types::FunVals&  cut ) = 0 ;
    /// set the C++ "fun-val" for LHCb::Vertex 
    virtual void set ( const LoKi::Types::VFunVals& cut ) = 0 ;
    // ========================================================================
    // cut-vals:
    // ========================================================================
    /// set the C++ "cut-val" for LHCb::Particle  
    virtual void set ( const LoKi::Types::CutVals&  cut ) = 0 ;
    /// set the C++ "cut-val" for LHCb::Vertex 
    virtual void set ( const LoKi::Types::VCutVals& cut ) = 0 ;
    // ========================================================================
    // elements:
    // ========================================================================
    /// set the C++ "element" for LHCb::Particle  
    virtual void set ( const LoKi::Types::Elements&  cut ) = 0 ;
    /// set the C++ "element" for LHCb::Vertex 
    virtual void set ( const LoKi::Types::VElements& cut ) = 0 ;
    // ========================================================================
    // sources:
    // ========================================================================
    /// set the C++ "source" for LHCb::Particle  
    virtual void set ( const LoKi::Types::Sources&  cut ) = 0 ;
    /// set the C++ "source" for LHCb::Vertex 
    virtual void set ( const LoKi::Types::VSources& cut ) = 0 ;
    // ========================================================================
  protected:
    // ========================================================================
    /// destructor : virtual and protected 
    virtual ~IHybridTool() ;              // destructor : virtual and protected 
    // ========================================================================
  private:
    // ========================================================================
  }; 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IHYBRIDTOOL_H
// ============================================================================
