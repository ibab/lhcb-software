// ============================================================================
#ifndef LOKI_ICOREANTIFACTORY_H 
#define LOKI_ICOREANTIFACTORY_H 1
// ============================================================================
// Include files
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/CoreTypes.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class ICoreAntiFactory
     *  The functor 'anti'-factory for creation of 'Core' LoKi functors 
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
    class ICoreAntiFactory : public virtual IAlgTool 
    {
    public: 
      // ======================================================================
      /// Return the unique interface ID
      static const InterfaceID& interfaceID() ;
      // ======================================================================
    public: // "void -> ... "
      // ======================================================================
      /// set the C++ predicate for 'void' 
      virtual void set ( const LoKi::Types::FCuts& cut ) = 0 ;
      /// set the C++ function  for 'void'
      virtual void set ( const LoKi::Types::FFunc& cut ) = 0 ;
      // ======================================================================
    public: // "double -> ... "
      // ======================================================================
      /// set the C++ predicate for 'double' 
      virtual void set ( const LoKi::Types::XCuts& cut ) = 0 ;
      /// set the C++ function  for 'double'
      virtual void set ( const LoKi::Types::XFunc& cut ) = 0 ;
      // ======================================================================
    public: // some functional stuff:
      // ======================================================================
      /// set: vector<double> -> vector<double> 
      virtual void set ( const LoKi::Types::XMaps&    cut ) = 0 ;
      /// set: vector<double> ->        double
      virtual void set ( const LoKi::Types::XFunVals& cut ) = 0 ;
      /// set: void  -> vector<double> 
      virtual void set ( const LoKi::Types::XSources& cut ) = 0 ;
      // ======================================================================
    protected:
      // ======================================================================
      /// destructor : virtual and protected 
      virtual ~ICoreAntiFactory() ;      // destructor : virtual and protected 
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::Hybrid  
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ICOREANTIFACTORY_H
// ============================================================================
