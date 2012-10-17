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
/** @file LoKi/CoreLock.h
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */
namespace LoKi
{
  // ==========================================================================
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
