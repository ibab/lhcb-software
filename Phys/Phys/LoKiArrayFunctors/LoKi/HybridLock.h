// $Id: HybridLock.h,v 1.1 2007-07-26 13:25:09 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_HYBRIDLOCK_H 
#define LOKI_HYBRIDLOCK_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/IHybridTool.h"
// ============================================================================
namespace LoKi
{
  namespace Hybrid
  {
    /** @class Lock  HybridLock.h LoKi/HybridLock.h 
     *  Helper class (sentry) to conhent IHynbdirTool to HybridEngine  
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09
     */
    class Lock 
    {
    public:
      // contructor : Lock 
      Lock  ( IHybridTool* tool ) ; ///< contructor : Lock 
      // destrcutor : UnLock
      virtual ~Lock () ;            ///< destrcutor : UnLock
    private:
      // no default constructor 
      Lock () ;                     //</ no default constructor 
      // no copy constructor 
      Lock ( const Lock& ) ; ///< no copy constructor 
      // no assignement opeartor 
      Lock& operator = ( const Lock& ) ; //< no assignement operator
    private:
      // the tool itself 
      LoKi::Interface<LoKi::IHybridTool> m_tool ;  ///< the tool itself 
    } ;
  } // end of namespace Hybrid 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HYBRIDLOCK_H
// ============================================================================
