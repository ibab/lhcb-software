// $Id: IHybridTool.h,v 1.2 2007-08-14 18:18:43 ibelyaev Exp $
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
    /// Return the unique interface ID
    static const InterfaceID& interfaceID() ;
  public:
    /// set the C++ predicate for LHCb::Particle
    virtual void set ( const LoKi::Types::Cuts&    cut ) = 0 ;    
    /// set the C++ predicate for LHCb::Vertex 
    virtual void set ( const LoKi::Types::VCuts&   cut ) = 0 ;    
    /// set the C++ predicate for arary of particles 
    virtual void set ( const LoKi::Types::ACuts&   cut ) = 0 ;    
    /// set the C++ function for LHCb::Particle  
    virtual void set ( const LoKi::Types::Func&    cut ) = 0 ;
    /// set the C++ function for LHCb::Vertex 
    virtual void set ( const LoKi::Types::VFunc&   cut ) = 0 ;
    /// set the C++ function for arary of particles 
    virtual void set ( const LoKi::Types::AFunc&   cut ) = 0 ;    
  protected:
    /// destructor : virtual and protected 
    virtual ~IHybridTool() ;
  private:
  }; 
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IHYBRIDTOOL_H
// ============================================================================
