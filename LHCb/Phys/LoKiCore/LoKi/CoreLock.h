// $Id: CoreLock.h,v 1.1 2008-10-19 16:11:40 ibelyaev Exp $
#ifndef LOKI_CORELOCK_H 
#define LOKI_CORELOCK_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/ICoreAntiFactory.h"
// ============================================================================
namespace LoKi
{
  namespace Hybrid
  {
    // ========================================================================
    /** @class CoreLock  LoKi/coreLock.h 
     *  Helper class (sentry) to connent LoKi::Hybrid::ICoreAntiFactory 
     *  to CoreEngine  
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-18
     */
    class CoreLock 
    {
    public:
      // ======================================================================
      /// contructor : Lock 
      CoreLock  ( LoKi::Hybrid::ICoreAntiFactory* tool ) ; // contructor : Lock 
      /// destrcutor : UnLock
      virtual ~CoreLock () ;                              // destructor : UnLock
      // ======================================================================
    private:
      // ======================================================================
      /// no default constructor 
      CoreLock () ;                                   // no default constructor 
      /// no copy constructor 
      CoreLock ( const CoreLock& ) ;                     // no copy constructor 
      /// no assignement opeartor 
      CoreLock& operator = ( const CoreLock& ) ;     // no assignement operator
      // ======================================================================
    private:
      // ======================================================================
      /// the tool itself 
      LoKi::Interface<LoKi::Hybrid::ICoreAntiFactory> m_tool ;  // the tool
      // ======================================================================
    } ; 
    // ========================================================================
 } // end of namespace LoKi::Hybrid 
  // ==========================================================================  
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CORELOCK_H
// ============================================================================
