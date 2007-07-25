// $Id: GenHybridLock.h,v 1.1 2007-07-25 15:07:57 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_GENHYBRIDLOCK_H 
#define LOKI_GENHYBRIDLOCK_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/IGenHybridTool.h"
// ============================================================================
namespace LoKi
{
  namespace Hybrid
  {
    /** @class GenLock  GenHybridLock.h LoKi/GenHybridLock.h 
     *  Helper class (sentry) to connent IGenHybridTool to GenEngine  
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09
     */
    class GenLock 
    {
    public:
      // contructor : Lock 
      GenLock  ( IGenHybridTool* tool ) ; ///< contructor : Lock 
      // destrcutor : UnLock
      virtual ~GenLock () ;            ///< destrcutor : UnLock
    private:
      // no default constructor 
      GenLock () ;                     //</ no default constructor 
      // no copy constructor 
      GenLock ( const GenLock& ) ; ///< no copy constructor 
      // no assignement opeartor 
      GenLock& operator = ( const GenLock& ) ; //< no assignement operator
    private:
      // the tool itself 
      LoKi::Interface<LoKi::IGenHybridTool> m_tool ;  ///< the tool itself 
    } ;
  } // end of namespace LoKi::Hybrid 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCHYBRIDLOCK_H
// ============================================================================
