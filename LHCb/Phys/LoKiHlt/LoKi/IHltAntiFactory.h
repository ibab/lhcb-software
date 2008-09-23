// $Id: IHltAntiFactory.h,v 1.2 2008-09-23 13:13:35 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_IHltANTIFACTORY_H 
#define LOKI_IHltANTIFACTORY_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/OdinTypes.h"
#include "LoKi/L0Types.h"
#include "LoKi/HLTTypes.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class IHltAntiFactory
     *  The functor 'anti'-factory for creation of Hlt-related  LoKi functors 
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
     *  @author Vanya BELYAEV Ivabn.Belyaev@nikhef.nl
     *  @date 2008-09-18
     */
    class IHltAntiFactory : public virtual IAlgTool 
    {
    public: 
      // ======================================================================
      /// Return the unique interface ID
      static const InterfaceID& interfaceID() ;
      // ======================================================================
    public: // L0
      // ======================================================================
      /// set the C++ function for LHCb::L0DUReport 
      virtual void set ( const LoKi::Types::L0_Cuts& cut ) = 0 ;
      /// set the C++ function for LHCb::L0DUReport
      virtual void set ( const LoKi::Types::L0_Func& cut ) = 0 ;
      // ======================================================================
    public: // O1 
      // ======================================================================
      /// set the C++ function for LHCb::ODIN
      virtual void set ( const LoKi::Types::ODIN_Cuts& cut ) = 0 ;
      /// set the C++ function for LHCb::ODIN
      virtual void set ( const LoKi::Types::ODIN_Func& cut ) = 0 ;
      // ======================================================================
    public: // HLT
      // ======================================================================
      /// set the C++ function for LHCb::HltDecReports
      virtual void set ( const LoKi::Types::HLT_Cuts& cut ) = 0 ;
      /// set the C++ function for LHCb::HltDecREports 
      virtual void set ( const LoKi::Types::HLT_Func& cut ) = 0 ;
      // ======================================================================
    protected:
      // ======================================================================
      /// destructor : virtual and protected 
      virtual ~IHltAntiFactory() ; // destructor : virtual and protected 
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::Hybrid  
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IHltANTIFACTORY_H
// ============================================================================
