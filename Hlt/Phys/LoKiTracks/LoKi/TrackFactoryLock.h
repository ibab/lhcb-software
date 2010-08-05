// $Id:$
// ============================================================================
#ifndef LOKI_TRACKHYBRIDLOCK_H 
#define LOKI_TRACKHYBRIDLOCK_H 1
// ============================================================================
// $URL$ 
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/ITrackFunctorAntiFactory.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class TrackFactoryLock LoKi/TrackFactoryLock.h 
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
    class TrackFactoryLock 
    {
    public:
      // ======================================================================
      /// contructor : Lock 
      TrackFactoryLock  ( LoKi::ITrackFunctorAntiFactory* tool ) ; 
      /// destrcutor : UnLock
      virtual ~TrackFactoryLock () ;                    // destrcutor : UnLock
      // ======================================================================
    private:
      // ======================================================================
      /// no default constructor 
      TrackFactoryLock () ;                     // no default constructor 
      /// no copy constructor 
      TrackFactoryLock ( const TrackFactoryLock& ) ; // no copy constructor 
      /// no assignement opeartor 
      TrackFactoryLock& operator = ( const TrackFactoryLock& ) ; 
      // ======================================================================
    private:
      // ======================================================================
      /// the tool itself 
      LoKi::Interface<LoKi::ITrackFunctorAntiFactory> m_tool ; // the tool 
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
