// $Id$
// ============================================================================
#ifndef LOKI_TRHYBRIDLOCK_H 
#define LOKI_TRHYBRIDLOCK_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/ITrHybridTool.h"
// ============================================================================
namespace LoKi
{
  namespace Hybrid 
  {
    /** @class TrLock  TrHybridLock.h LoKi/TrHybridLock.h 
     *  Helper class (sentry) to connect IHybridTool to HybridEngine  
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09
     */
    class TrLock 
    {
    public:
      // contructor : Lock 
      TrLock  ( ITrHybridTool* tool ) ; ///< contructor : Lock 
      // destrcutor : UnLock
      virtual ~TrLock () ;              ///< destrcutor : UnLock
    private:
      // no default constructor 
      TrLock () ;                     //</ no default constructor 
      // no copy constructor 
      TrLock ( const TrLock& ) ; ///< no copy constructor 
      // no assignement opeartor 
      TrLock& operator = ( const TrLock& ) ; //< no assignement operator
    private:
      // the tool itself 
      LoKi::Interface<LoKi::ITrHybridTool> m_tool ;  ///< the tool itself 
    } ;
  } // end of namespace LoKi::Hybrid 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HYBRIDLOCK_H
// ============================================================================
