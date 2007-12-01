// $Id: IGenHybridTool.h,v 1.2 2007-12-01 13:52:13 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_IGENHYBRIDTOOL_H 
#define LOKI_IGENHYBRIDTOOL_H 1
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
#include "LoKi/GenTypes.h"
// ============================================================================
namespace LoKi
{ 
  // ==========================================================================
  /** @class IGenHybridTool IMCHybridTool.h LoKi/IMCHybridTool.h
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
  class IGenHybridTool : public virtual IAlgTool 
  {
  public:
    /// Return the unique interface ID
    static const InterfaceID& interfaceID() ;
  public:
    /// set the C++ predicate for HepMC::GenParticle
    virtual void set ( const LoKi::Types::GCuts&   cut ) = 0 ;    
    /// set the C++ predicate for HepMC::GenVertex
    virtual void set ( const LoKi::Types::GVCuts&  cut ) = 0 ;
    /// set the C++ function for HepMC::GenParticle 
    virtual void set ( const LoKi::Types::GFunc&   cut ) = 0 ;
    /// set the C++ function for HepMC::GenVertex
    virtual void set ( const LoKi::Types::GVFunc&  cut ) = 0 ;
  protected:
    /// destructor : virtual and protected 
    virtual ~IGenHybridTool() ;
  private:
  };
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IGENHYBRIDTOOL_H
// ============================================================================
