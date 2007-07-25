// $Id: IMCHybridTool.h,v 1.1 2007-07-25 15:16:06 ibelyaev Exp $
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
  class IMCHybridTool : public virtual IAlgTool 
  {
  public:
    /// Return the unique interface ID
    static const InterfaceID& interfaceID() ;
  public:
    /// set the C++ predicate for LHCb::MCParticle
    virtual void set ( const LoKi::Types::MCCuts&  cut ) = 0 ;    
    /// set the C++ predicate for LHCb::MCVertex 
    virtual void set ( const LoKi::Types::MCVCuts& cut ) = 0 ;    
    /// set the C++ function for LHCb::MCParticle 
    virtual void set ( const LoKi::Types::MCFunc&  cut ) = 0 ;
    /// set the C++ function for LHCb::MCVertex 
    virtual void set ( const LoKi::Types::MCVFunc& cut ) = 0 ;
  protected:
    /// destructor : virtual and protected 
    virtual ~IMCHybridTool() ;
  private:
  };
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IMCHYBRIDTOOL_H
// ============================================================================
