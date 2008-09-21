// $Id: O1Cuts.h,v 1.1.1.1 2008-09-21 14:41:20 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_O1CUTS_H 
#define LOKI_O1CUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/O1.h"
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
    /** @var O1_ALL
     *  the primitive predicate ('accept all')
     *  @see LoKi::Constant
     *  @see LHCb::ODIN
     *  @see LoKi::Cuts::O1TRUE
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Constant<const LHCb::ODIN*,bool>              O1_ALL ( true ) ;
    // ========================================================================
    /** @var O1_BUNCH
     *  Simple accessor to bunch id 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int bunchID = O1_BUNCH ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::O1::BunchId
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::bunchId
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::O1::BunchId                                          O1_BUNCH ;
    // ========================================================================
    /** @var O1_BXCURRENT
     *  Simple accessor to bunch crossing current 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int current = O1_BXCURRENT ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::O1::BunchCurrent
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::bunchCurrent
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::O1::BunchCurrent                                 O1_BXCURRENT ;
    // ========================================================================
    /** @typedef O1_BXID
     *  simple checker of bx-id, bx-range or bx-list
     *  @code 
     *  
     *   const LHCb::ODIN* odin = ... ;
     * 
     *   const O1_BXID goodBx    = O1_BXID ( 16 ) ;
     *   const O1_BXID goodRange = O1_BXID ( 18 , 24  ) ;
     *   std::vector<unsigned int> lst = ... 
     *   const O1_BXID goodList  = O1_BXID ( lst ) ;
     *     
     *  @endcode 
     * 
     *  @see LoKi::O1::BxId
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::bunchId
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    typedef LoKi::O1::BXId                                            O1_BXID ;
    // ========================================================================
    /** @var O1_BXTYP
     *  Simple accessor to bunch crossing type 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int bxType = O1_BXTYP ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::O1::BXType
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::bunchCrossingType
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::O1::BXType                                           O1_BXTYP ;
    // ========================================================================
    /** @var O1_CALSTEP
     *  Simple accessor to calibration step
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int step = O1_CALSTEP ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::O1::CalibrationStep
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::calibrationStep
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::O1::CalibrationStep                                O1_CALSTEP ;
    // ========================================================================
    /** @var O1_FORCEBIT
     *  Simple accessor to force bit 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const bool force = O1_FORCEBIT ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::O1::ForceBit 
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::forceBit 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::O1::ForceBit                                      O1_FORCEBIT ;
    // ========================================================================
    /** @var O1_EVTTYP
     *  Simple accessor to event type 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int evtType = O1_EVTTYP ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::O1::EventType
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::eventType
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::O1::EventType                                       O1_EVTTYP ;
    // ========================================================================
    /** @var O1_ERRBITS
     *  Simple accessor to error bits 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int errBits = O1_ERRBITS ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::O1::ErrorBits
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::errorBits
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::O1::ErrorBits                                      O1_ERRBITS ;
    // ========================================================================
    /** @var O1_FALSE
     *  the primitive predicate ('accept nothing')
     *  @see LoKi::Constant
     *  @see LHCb::ODIN
     *  @see LoKi::Cuts::O1_NONE
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Constant<const LHCb::ODIN*,bool>           O1_FALSE ( false ) ;
    // ========================================================================
    /** @var O1_NONE
     *  the primitive predicate ('accept nothing')
     *  @see LoKi::Constant
     *  @see LHCb::ODIN
     *  @see LoKi::Cuts::O1_FALSE
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Constant<const LHCb::ODIN*,bool>            O1_NONE ( false ) ;
    // ========================================================================
    /** @var O1_NULL
     *  the primitive function (==0)
     *  @see LoKi::Constant
     *  @see LHCb::ODIN
     *  @see LoKi::Cuts::O1_ZERO
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Constant<const LHCb::ODIN*,double>              O1_NULL ( 0 ) ;
    // ========================================================================
    /** @var O1_ONE
     *  the primitive function (==1)
     *  @see LoKi::Constant
     *  @see LHCb::ODIN
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Constant<const LHCb::ODIN*,double>               O1_ONE ( 1 ) ;
    // ========================================================================
    /** @var O1_ORBIT
     *  Simple accessor to orbit 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int orbit = O1_ORBIT ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::O1::Orbit
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::orbitNumber 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::O1::Orbit                                            O1_ORBIT ;
    // ========================================================================
    /** @var O1_ROTYP
     *  Simple accessor to readout type 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int roType = O1_ROTYP ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::O1::ReadOutType
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::readoutType
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::O1::ReadOutType                                      O1_ROTYP ;
    // ========================================================================
    /** @var O1_RUN
     *  Simple accessor to run number 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const bool goodRun  = 15389 == O1_RUN( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::O1::Run
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::runNumber 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::O1::Run                                                O1_RUN ;
    // ========================================================================
    /** @typedef O1_RUNNUMBER 
     *  simple checker of run, run range and run list:
     *  @code 
     *  
     *   const LHCb::ODIN* odin = ... ;
     * 
     *   const O1_RUNNUMBER goodRun   = O1_RUNNUMBER ( 345234 ) ;
     *   const O1_RUNNUMBER goodRange = O1_RUNNUMBER ( 12143 , 2345135 ) ;
     *   std::vector<unsigned int> lst = ... 
     *   const O1_RUNNUMBER goodList  = O1_RUNNUMBER ( lst ) ;
     *     
     *  @endcode 
     * 
     *  @see LoKi::O1::Run
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::runNumber 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    typedef LoKi::O1::RunNumber                                  O1_RUNNUMBER ;
    // ========================================================================    
    /** @var O1_TCK
     *  Simple accessor to trigger configuration key 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int key = O1_TCK ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::O1::TrgConfKey
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::triggerConfigurationKey
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::O1::TrgConfKey                                         O1_TCK ;
    // ========================================================================
    /** @typedef O1_TCKEYS 
     *  simple checker of tck, tck-range or tck-list 
     *  @code 
     *  
     *   const LHCb::ODIN* odin = ... ;
     * 
     *   const O1_TCKEYS goodTck   =  O1_TCKEYS( 345234 ) ;
     *   const O1_TCKEYS goodRange =  O1_TCKEYS ( 12143 , 2345135 ) ;
     *   std::vector<unsigned int> lst = ... 
     *   const O1_TCKEYS goodList  =  O1_TCKEYS ( lst ) ;
     *     
     *  @endcode 
     * 
     *  @see LoKi::O1::Tck
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::triggerConfigurationKey
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    typedef LoKi::O1::Tck                                           O1_TCKEYS ;
    // ========================================================================
    /** @typedef O1_TIME
     *  Simple check for the time interval for the given event 
     *  
     *  @code
     *  
     *    const Gaudi::Time& start = ... ;
     *    const Gaudi::Time& stop  = ... ;
     *
     *    // create the functor 
     *    const O1_TIME ok ( start, stop ) ;
     * 
     *    const LHCb::ODIN* odin = ... ; 
     *
     *    const bool inTime = ok ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::O1::InTime
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::EventTime
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    typedef LoKi::O1::InTime                                          O1_TIME ;   
    // ========================================================================
    /** @var O1_TRGTYP
     *  Simple accessor to trigger type 
     *  @code
     * 
     *  const LHCb::ODIN* odin = ... ;
     * 
     *  const unsigned int trgType = O1_TRGTYP ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::O1::TriggerType
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::triggerType
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::O1::TriggerType                                     O1_TRGTYP ;
    // ========================================================================
    /** @var O1_TRUE
     *  the primitive predicate ('accept all')
     *  @see LoKi::Constant
     *  @see LHCb::ODIN
     *  @see LoKi::Cuts::O1_ALL
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
     const LoKi::Constant<const LHCb::ODIN*,bool>            O1_TRUE ( true ) ;
    // ========================================================================
    /** @var O1_ZERO
     *  the primitive function (==0)
     *  @see LoKi::Constant
     *  @see LHCb::ODIN
     *  @see LoKi::Cuts::O1_NULL
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    const LoKi::Constant<const LHCb::ODIN*,double>              O1_ZERO ( 0 ) ;
    // ========================================================================
  } // end of namespace LoKi::Cuts 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_O1CUTS_H
// ============================================================================
