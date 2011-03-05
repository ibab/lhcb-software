// $Id$
// ============================================================================
#ifndef LOKI_TRGLOCK_H 
#define LOKI_TRGLOCK_H 1
// ============================================================================
// $URL$ 
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/ITrgFunctorAntiFactory.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class TrgFactoryLock LoKi/TrgFactoryLock.h 
     *
     *  Helper class (sentry) to connect ITrackFunctorAntiFactory to TrackEngine 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-09
     *
     *  $Revision$
     *  Last Modification $Date$ 
     *                 by $Author$ 
     *
     */
    class TrgFactoryLock 
    {
    public:
      // ======================================================================
      /// contructor : Lock 
      TrgFactoryLock  ( LoKi::ITrgFunctorAntiFactory* tool ) ; 
      /// destrcutor : UnLock
      virtual ~TrgFactoryLock () ;                    // destrcutor : UnLock
      // ======================================================================
    private:
      // ======================================================================
      /// no default constructor 
      TrgFactoryLock () ;                     // no default constructor 
      /// no copy constructor 
      TrgFactoryLock ( const TrgFactoryLock& ) ; // no copy constructor 
      /// no assignement opeartor 
      TrgFactoryLock& operator = ( const TrgFactoryLock& ) ; 
      // ======================================================================
    private:
      // ======================================================================
      /// the tool itself 
      LoKi::Interface<LoKi::ITrgFunctorAntiFactory> m_tool ;        // the tool 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                            end of namespace LoKi::Hybrid 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HYBRIDLOCK_H
// ============================================================================
