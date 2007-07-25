// $Id: MCHybridLock.h,v 1.1 2007-07-25 15:16:06 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MCHYBRIDLOCK_H 
#define LOKI_MCHYBRIDLOCK_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/IMCHybridTool.h"
// ============================================================================
namespace LoKi
{
  namespace Hybrid 
  {
    /** @class MCHybridLock  MCHybriddLock.h LoKi/MCHybridLock.h 
     *  Helper class (sentry) to connent IMCHybridTool to MCHybridEngine  
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09
     */
    class MCLock 
    {
    public:
      // contructor : Lock 
      MCLock  ( IMCHybridTool* tool ) ; ///< contructor : Lock 
      // destrcutor : UnLock
      virtual ~MCLock () ;            ///< destrcutor : UnLock
    private:
      // no default constructor 
      MCLock () ;                     //</ no default constructor 
      // no copy constructor 
      MCLock ( const MCLock& ) ; ///< no copy constructor 
      // no assignement opeartor 
      MCLock& operator = ( const MCLock& ) ; //< no assignement operator
    private:
      // the tool itself 
      LoKi::Interface<LoKi::IMCHybridTool> m_tool ;  ///< the tool itself 
    } ;
  }
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCHYBRIDLOCK_H
// ============================================================================
