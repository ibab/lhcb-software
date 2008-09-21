// $Id: HltLock.h,v 1.1.1.1 2008-09-21 14:41:20 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_HltLOCK_H 
#define LOKI_HltLOCK_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/IHltAntiFactory.h"
// ============================================================================
namespace LoKi
{
  namespace Hybrid
  {
    // ========================================================================
    /** @class HltLock  LoKi/HltLock.h 
     *  Helper class (sentry) to connent LoKi::Hybrid::IHltAntFactory 
     *  to HltEngine  
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-18
     */
    class HltLock 
    {
    public:
      // ======================================================================
      /// contructor : Lock 
      HltLock  ( LoKi::Hybrid::IHltAntiFactory* tool ) ; // contructor : Lock 
      /// destrcutor : UnLock
      virtual ~HltLock () ;                              // destructor : UnLock
      // ======================================================================
    private:
      // ======================================================================
      /// no default constructor 
      HltLock () ;                                    // no default constructor 
      /// no copy constructor 
      HltLock ( const HltLock& ) ;                       // no copy constructor 
      /// no assignement opeartor 
      HltLock& operator = ( const HltLock& ) ;       // no assignement operator
      // ======================================================================
    private:
      // ======================================================================
      /// the tool itself 
      LoKi::Interface<LoKi::Hybrid::IHltAntiFactory> m_tool ;  // the tool
      // ======================================================================
    } ; 
    // ========================================================================
 } // end of namespace LoKi::Hybrid 
  // ==========================================================================  
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HltLOCK_H
// ============================================================================
