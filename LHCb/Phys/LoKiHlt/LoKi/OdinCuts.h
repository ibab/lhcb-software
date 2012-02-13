// $Id: OdinCuts.h,v 1.3 2010-05-17 16:01:39 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_O1CUTS_H 
#define LOKI_O1CUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Odin.h"
#include "LoKi/Prescale.h"
// ============================================================================
/** @file
 *  The definition of useful symbols for 'Odin'-functions
 *  @see LHCb::ODIN
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-09-17
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @var ODIN_ALL
     *  the primitive predicate ('accept all')
     *  @see LoKi::Constant
     *  @see LHCb::ODIN
     *  @see LoKi::Cuts::O1TRUE
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Constant<const LHCb::ODIN*,bool>            ODIN_ALL ( true ) ;
    // ========================================================================
    /** @var ODIN_BUNCH
     *  Simple accessor to bunch id 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int bunchID = ODIN_BUNCH ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Odin::BunchId
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::bunchId
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Odin::BunchId                                      ODIN_BUNCH ;
    // ========================================================================
    /** @var ODIN_BXCURRENT
     *  Simple accessor to bunch crossing current 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int current = ODIN_BXCURRENT ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Odin::BunchCurrent
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::bunchCurrent
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Odin::BunchCurrent                             ODIN_BXCURRENT ;
    // ========================================================================
    /** @typedef ODIN_BXID
     *  simple checker of bx-id, bx-range or bx-list
     *  @code 
     *  
     *   const LHCb::ODIN* odin = ... ;
     * 
     *   const ODIN_BXID goodBx    = ODIN_BXID ( 16 ) ;
     *   const ODIN_BXID goodRange = ODIN_BXID ( 18 , 24  ) ;
     *   std::vector<unsigned int> lst = ... 
     *   const ODIN_BXID goodList  = ODIN_BXID ( lst ) ;
     *     
     *  @endcode 
     * 
     *  @see LoKi::Odin::BxId
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::bunchId
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    typedef LoKi::Odin::BXId                                        ODIN_BXID ;
    // ========================================================================
    /** @var ODIN_BXTYP
     *  Simple accessor to bunch crossing type 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int bxType = ODIN_BXTYP ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Odin::BXType
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::bunchCrossingType
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Odin::BXType                                       ODIN_BXTYP ;
    // ========================================================================
    /** @var ODIN_CALSTEP
     *  Simple accessor to calibration step
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int step = ODIN_CALSTEP ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Odin::CalibrationStep
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::calibrationStep
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Odin::CalibrationStep                            ODIN_CALSTEP ;
    // ========================================================================
    /** @var ODIN_FORCEBIT
     *  Simple accessor to force bit 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const bool force = ODIN_FORCEBIT ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Odin::ForceBit 
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::forceBit 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Odin::ForceBit                                  ODIN_FORCEBIT ;
    // ========================================================================
    /** @var ODIN_EVTTYP
     *  Simple accessor to event type 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int evtType = ODIN_EVTTYP ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Odin::EventType
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::eventType
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Odin::EventType                                   ODIN_EVTTYP ;
    // ========================================================================
    /** @var ODIN_EVTNUMBER
     *  Simple checkfor event number 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  // event number 
     *  const ODIN_EVTNUMBER n1 = ODIN_EVTNUMBER ( 1 ) ;
     * 
     *  // event range 
     *  const ODIN_EVTNUMBER n2 = ODIN_EVTNUMBER ( 1 , 10 ) ;
     *
     *  // event list
     *  LoKi::Numbers::EvtNumList lst = ... 
     *  const ODIN_EVTNUMBER n3 = ODIN_EVTNUMBER ( lst ) ;
     * 
     *  // use it!
     *  
     *  const bool ok1 = n1 ( odin ) ;
     *  const bool ok2 = n2 ( odin ) ;
     *  const bool ok3 = n3 ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Odin::EvtNumber 
     *  @see LHCb::ODIN
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2012-02-13     
     */
    typedef LoKi::Odin::EvtNumber                              ODIN_EVTNUMBER ;
    // ========================================================================
    /** @var ODIN_RUNEVT
     *  Simple checkfor event number 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  // event number 
     *  LoKi::Numbers::RunEvt e1 = ... ;
     *  const ODIN_RUNEVT n1 = ODIN_RUNEVT ( e1 ) ;
     * 
     *  // event range 
     *  LoKi::Numbers::RunEvt begin = ... ;
     *  LoKi::Numbers::RunEvt end   = ... ;
     *  const ODIN_RUNEVT n2 = ODIN_RUNEVT ( begin , end  ) ;
     *
     *  // event list
     *  LoKi::Numbers::RunEvtList lst = ... 
     *  const ODIN_RUNEVT n3 = ODIN_RUNEVT ( lst ) ;
     * 
     *  // use it!
     *  
     *  const bool ok1 = n1 ( odin ) ;
     *  const bool ok2 = n2 ( odin ) ;
     *  const bool ok3 = n3 ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Odin::RunEvtNumber 
     *  @see LHCb::ODIN
     *  @see LoKi::Cuts::ODIN_RUNEVTNUMBER 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2012-02-13     
     */
    typedef LoKi::Odin::RunEvtNumber                              ODIN_RUNEVT ;
    // ========================================================================
    /** @var ODIN_RUNEVT
     *  Simple checkfor event number 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  // event number 
     *  LoKi::Numbers::RunEvt e1 = ... ;
     *  const ODIN_RUNEVTNUMBER n1 = ODIN_RUNEVTNUMBER ( e1 ) ;
     * 
     *  // event range 
     *  LoKi::Numbers::RunEvt begin = ... ;
     *  LoKi::Numbers::RunEvt end   = ... ;
     *  const ODIN_RUNEVTNUMBER n2 = ODIN_RUNEVTNUMBER ( begin , end  ) ;
     *
     *  // event list
     *  LoKi::Numbers::RunEvtList lst = ... 
     *  const ODIN_RUNEVTNUMBER n3 = ODIN_RUNEVTNUMBER ( lst ) ;
     * 
     *  // use it!
     *  
     *  const bool ok1 = n1 ( odin ) ;
     *  const bool ok2 = n2 ( odin ) ;
     *  const bool ok3 = n3 ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Odin::RunEvtNumber  
     *  @see LoKi::Cuts::ODIN_RUNEVT 
     *  @see LHCb::ODIN
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2012-02-13     
     */
    typedef LoKi::Odin::RunEvtNumber                        ODIN_RUNEVTNUMBER ;
    // ========================================================================
    /** @var ODIN_ERRBITS
     *  Simple accessor to error bits 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int errBits = ODIN_ERRBITS ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Odin::ErrorBits
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::errorBits
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Odin::ErrorBits                                  ODIN_ERRBITS ;
    // ========================================================================
    /** @var ODIN_FALSE
     *  the primitive predicate ('accept nothing')
     *  @see LoKi::Constant
     *  @see LHCb::ODIN
     *  @see LoKi::Cuts::ODIN_NONE
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Constant<const LHCb::ODIN*,bool>         ODIN_FALSE ( false ) ;
    // ========================================================================
    /** @var ODIN_NONE
     *  the primitive predicate ('accept nothing')
     *  @see LoKi::Constant
     *  @see LHCb::ODIN
     *  @see LoKi::Cuts::ODIN_FALSE
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Constant<const LHCb::ODIN*,bool>          ODIN_NONE ( false ) ;
    // ========================================================================
    /** @var ODIN_NULL
     *  the primitive function (==0)
     *  @see LoKi::Constant
     *  @see LHCb::ODIN
     *  @see LoKi::Cuts::ODIN_ZERO
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Constant<const LHCb::ODIN*,double>            ODIN_NULL ( 0 ) ;
    // ========================================================================
    /** @var ODIN_ONE
     *  the primitive function (==1)
     *  @see LoKi::Constant
     *  @see LHCb::ODIN
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Constant<const LHCb::ODIN*,double>             ODIN_ONE ( 1 ) ;
    // ========================================================================
    /** @var ODIN_ORBIT
     *  Simple accessor to orbit 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int orbit = ODIN_ORBIT ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Odin::Orbit
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::orbitNumber 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Odin::Orbit                                            ODIN_ORBIT ;
    // ========================================================================
    /** @typedef ODIN_PRESCALE
     *  Simple deterministic prescaler.
     * 
     *  @code
     * 
     *  const ODIN_PRESCALE scale  = ODIN_PRESCALE ( 0.01 , 'some_see_here' ) ;
     *
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const bool accept = scale ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Odin::Prescale
     *  @see DeterministicPrescaler 
     *
     *  The idea & code belongs to Gerhard The Great Raven
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-02-12
     */
    typedef LoKi::Odin::Prescale                                ODIN_PRESCALE ;
    // ========================================================================
    /** @var ODIN_ROTYP
     *  Simple accessor to readout type 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int roType = ODIN_ROTYP ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Odin::ReadOutType
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::readoutType
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Odin::ReadOutType                                  ODIN_ROTYP ;
    // ========================================================================
    /** @typedef ODIN_ROUTINGBITS 
     *  Simple functor tocheck the routing bits 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const ODIN_ROUTINGBITS bits = ODIN_ROUTINGBITS ( ROUTINGBITS ( 1, 2, 3  ) ) ;
     *
     *  const bool ok = bits ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Odin::BunchId
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::bunchId
     *  @see LHCb::Cuts::ROUTINGBITS 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    typedef LoKi::Odin::RoutingBits                          ODIN_ROUTINGBITS ;
    // ========================================================================
    /** @var ODIN_RUN
     *  Simple accessor to run number 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const bool goodRun  = 15389 == ODIN_RUN( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Odin::Run
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::runNumber 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Odin::Run                                            ODIN_RUN ;
    // ========================================================================
    /** @typedef ODIN_RUNNUMBER 
     *  simple checker of run, run range and run list:
     *  @code 
     *  
     *   const LHCb::ODIN* odin = ... ;
     * 
     *   const ODIN_RUNNUMBER goodRun   = ODIN_RUNNUMBER ( 345234 ) ;
     *   std::vector<unsigned int> lst = ... 
     *   const ODIN_RUNNUMBER goodList  = ODIN_RUNNUMBER ( lst ) ;
     *     
     *  @endcode 
     * 
     *  @see LoKi::Odin::Run
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::runNumber 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    typedef LoKi::Odin::RunNumber                              ODIN_RUNNUMBER ;
    // ========================================================================    
    /** @var ODIN_TCK
     *  Simple accessor to trigger configuration key 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int key = ODIN_TCK ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Odin::TrgConfKey
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::triggerConfigurationKey
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Odin::TrgConfKey                                     ODIN_TCK ;
    // ========================================================================
    /** @typedef ODIN_TCKEYS 
     *  simple checker of tck, tck-range or tck-list 
     *  @code 
     *  
     *   const LHCb::ODIN* odin = ... ;
     * 
     *   const ODIN_TCKEYS goodTck   =  ODIN_TCKEYS( 345234 ) ;
     *   const ODIN_TCKEYS goodRange =  ODIN_TCKEYS ( 12143 , 2345135 ) ;
     *   std::vector<unsigned int> lst = ... 
     *   const ODIN_TCKEYS goodList  =  ODIN_TCKEYS ( lst ) ;
     *     
     *  @endcode 
     * 
     *  @see LoKi::Odin::Tck
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::triggerConfigurationKey
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    typedef LoKi::Odin::Tck                                       ODIN_TCKEYS ;
    // ========================================================================
    /** @typedef ODIN_TIME
     *  Simple check for the time interval for the given event 
     *  
     *  @code
     *  
     *    const Gaudi::Time& start = ... ;
     *    const Gaudi::Time& stop  = ... ;
     *
     *    // create the functor 
     *    const ODIN_TIME ok ( start, stop ) ;
     * 
     *    const LHCb::ODIN* odin = ... ; 
     *
     *    const bool inTime = ok ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Odin::InTime
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::EventTime
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    typedef LoKi::Odin::InTime                                      ODIN_TIME ;   
    // ========================================================================
    /** @var ODIN_TRGTYP
     *  Simple accessor to trigger type 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int trgType = ODIN_TRGTYP ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Odin::TriggerType
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::triggerType
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Odin::TriggerType                                 ODIN_TRGTYP ;
    // ========================================================================
    /** @var ODIN_TRUE
     *  the primitive predicate ('accept all')
     *  @see LoKi::Constant
     *  @see LHCb::ODIN
     *  @see LoKi::Cuts::ODIN_ALL
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
     const LoKi::Constant<const LHCb::ODIN*,bool>          ODIN_TRUE ( true ) ;
    // ========================================================================
    /** @var ODIN_ZERO
     *  the primitive function (==0)
     *  @see LoKi::Constant
     *  @see LHCb::ODIN
     *  @see LoKi::Cuts::ODIN_NULL
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Constant<const LHCb::ODIN*,double>            ODIN_ZERO ( 0 ) ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_O1CUTS_H
// ============================================================================
