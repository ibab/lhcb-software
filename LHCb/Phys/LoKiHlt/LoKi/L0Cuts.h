// $Id: L0Cuts.h,v 1.2 2010-01-08 13:30:47 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_L0CUTS_H 
#define LOKI_L0CUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/L0.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Cuts 
  {   
    // ========================================================================
    /** @typedef L0_CHANNEL
     *  Accessor to check the channel decision 
     * 
     *  @code 
     * 
     *   const LHCb::L0DUReport* l0 = ... ;
     * 
     *   const std::vector<std::string>& channels = ... ; 
     *
     *   const L0_CHANNEL d1 = L0_CHANNEL (  1             , 0 ) ;
     *   const L0_CHANNEL d2 = L0_CHANNEL ( 'channel name' , 0 ) ;
     *   const L0_CHANNEL d2 = L0_CHANNEL (  channels      , 0 ) ;
     * 
     *   const bool dec1 = d1 ( l0 ) ;
     *   const bool dec2 = d2 ( l0 ) ;
     *   const bool dec3 = d3 ( l0 ) ;
     *
     *  @endcode 
     *     
     *  @see LoKi::Cuts::L0_CHDECISION
     *  @see LoKi::L0::ChannelDecision
     *  @see LHCb::L0DUReport::channelDecision 
     *  @see LHCb::L0DUReport::channelDecisionByName  
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    typedef LoKi::L0::ChannelDecision                              L0_CHANNEL ;
    // ========================================================================
    /** @typedef L0_CHDECISION
     *  Accessor to check the channel decision 
     * 
     *  @code 
     * 
     *   const LHCb::L0DUReport* l0 = ... ;
     * 
     *   const std::vector<std::string>& channels = ... ; 
     *
     *   const L0_CHDECISION d1 = L0_CHDECISION (  1             , 0 ) ;
     *   const L0_CHDECISION d2 = L0_CHDECISION ( 'channel name' , 0 ) ;
     *   const L0_CHDECISION d3 = L0_CHDECISION (  channels      , 0 ) ;
     * 
     *   const bool dec1 = d1 ( l0 ) ;
     *   const bool dec2 = d2 ( l0 ) ;
     *   const bool dec3 = d3 ( l0 ) ;
     *
     *  @endcode 
     *     
     *  @see LoKi::Cuts::L0_CHANNEL
     *  @see LoKi::L0::ChannelDecision
     *  @see LHCb::L0DUReport::channelDecision 
     *  @see LHCb::L0DUReport::channelDecisionByName  
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    typedef LoKi::L0::ChannelDecision                           L0_CHDECISION ;
    // ========================================================================
    /** @typedef L0_CHPREDECISION
     *  Accessor to check the channel PRE-decision 
     * 
     *  @code 
     * 
     *   const LHCb::L0DUReport* l0 = ... ;
     * 
     *   const std::vector<std::string>& channels = ... ; 
     *
     *   const L0_CHPREDECISION d1 = L0_CHPREDECISION (  1             , 0 ) ;
     *   const L0_CHPREDECISION d2 = L0_CHPREDECISION ( 'channel name' , 0 ) ;
     *   const L0_CHPREDECISION d2 = L0_CHPREDECISION (  channels      , 0 ) ;
     * 
     *   const bool dec1 = d1 ( l0 ) ;
     *   const bool dec2 = d2 ( l0 ) ;
     *   const bool dec3 = d3 ( l0 ) ;
     *
     *  @endcode 
     *     
     *  @see LoKi::L0::ChannelPreDecision
     *  @see LHCb::L0DUReport::channelPreDecision 
     *  @see LHCb::L0DUReport::channelPreDecisionByName  
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    typedef LoKi::L0::ChannelPreDecision                      L0_CHPREDECISION ;
    // ========================================================================
    /** @typedef L0_CONDITION
     *  Accessor to check the condition value 
     * 
     *  @code 
     * 
     *   const LHCb::L0DUReport* l0 = ... ;
     * 
     *   const std::vector<std::string>& conditions = ... ; 
     *
     *   const L0_CONDITION c1 = L0_CONDITION (  1               , 0 ) ;
     *   const L0_CONDITION c2 = L0_CONDIITON ( 'condiiton name' , 0 ) ;
     *   const L0_CONDITION c3 = L0_CONDITION (  conditions      , 0 ) ;
     * 
     *   const bool con1 = c1 ( l0 ) ;
     *   const bool con2 = c2 ( l0 ) ;
     *   const bool con3 = c3 ( l0 ) ;
     *
     *  @endcode 
     *     
     *  @see LoKi::L0::ConditionValue
     *  @see LHCb::L0DUReport::conditionValue
     *  @see LHCb::L0DUReport::conditionValueByName  
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    typedef LoKi::L0::ConditionValue                             L0_CONDITION ;
    // ========================================================================
    /** @typedef L0_DATA
     *  Simple accessor to 'dataValue' for L0 
     * 
     *  @code 
     * 
     *   const LHCb::L0DUReport* l0 = ... ;
     * 
     *   const std::string& name = ... ;
     *
     *   const L0_DATA data = L0_DATA ( name  ) ;
     *  
     *   double value = data ( l0 ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::L0::DataValue
     *  @see LHCb::L0DUReport::dataValue  
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    typedef  LoKi::L0::DataValue                                      L0_DATA ;
    // ========================================================================
    /** @var L0_DECISION
     *  Simple accessor to 'global decision''
     * 
     *  @code 
     * 
     *   const LHCb::L0DUReport* l0 = ... ;
     * 
     *   bool decision = L0_DECISION ( l0 ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::L0::Decision
     *  @see LHCb::L0DUReport::decision
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    const LoKi::L0::Decision                                      L0_DECISION ;
    // ========================================================================
    /** @typedef L0_DIGIT
     *  Simple accessor to 'dataDigit' for L0 
     * 
     *  @code 
     * 
     *   const LHCb::L0DUReport* l0 = ... ;
     * 
     *   const std::string& name = ... ;
     *
     *   const L0_DIGIT digit = L0_DIGIT ( name  ) ;
     *  
     *   double value = digit  ( l0 ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::L0::DataDigit
     *  @see LHCb::L0DUReport::dataDigit
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    typedef  LoKi::L0::DataDigit                                     L0_DIGIT ;
    // ========================================================================
    /** @var L0_FORCEBIT
     *  Simple accessor to 'force bit'
     * 
     *  @code 
     * 
     *   const LHCb::L0DUReport* l0 = ... ;
     * 
     *   bool force = L0_FORCEBIT( l0 ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::L0::ForceBit
     *  @see LHCb::L0DUReport::forceBit
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    const LoKi::L0::ForceBit                                      L0_FORCEBIT ;
    // ========================================================================
    /** @typedef L0_SUMET 
     *  Simple accessor to 'sum-et' by L0 
     * 
     *  @code 
     * 
     *   const LHCb::L0DUReport* l0 = ... ;
     * 
     *   const L0_SUMET sumEt = L0_SUMET( 0 ) ;
     *  
     *   double energy = sumEt( l0 ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::L0_SUMET0
     *  @see LoKi::L0::SumEt
     *  @see LHCb::L0DUReport::sumEt 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    typedef  LoKi::L0::SumEt                                         L0_SUMET ;
    // ========================================================================
    /** @var L0_SUMET0 
     *  Simple accessor to 'sum-et' by L0 in bx=0
     * 
     *  @code 
     * 
     *   const LHCb::L0DUReport* l0 = ... ;
     * 
     *   double energy = L0_SUMET0 ( l0 ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::L0_SUMET
     *  @see LoKi::L0::SumEt
     *  @see LHCb::L0DUReport::sumEt 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    const LoKi::L0::SumEt                                           L0_SUMET0 ;
    // ========================================================================
    /** @var L0_SUMDECISON
     *  Simple accessor to 'decison form summary'
     * 
     *  @code 
     * 
     *   const LHCb::L0DUReport* l0 = ... ;
     * 
     *   bool decision = L0_SUMDECISION ( l0 ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::L0::SumDecision
     *  @see LHCb::L0DUReport::decisionFroSummary
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    const LoKi::L0::SumDecision                                L0_SUMDECISION ;
    // ========================================================================
    /** @var L0_TCK
     *  Simple accessor to 'trigger configuration key'
     * 
     *  @code 
     * 
     *   const LHCb::L0DUReport* l0 = ... ;
     * 
     *   double tck = L0_TCK ( l0 ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::L0::Tck
     *  @see LHCb::L0DUReport::tck
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    const LoKi::L0::Tck                                                L0_TCK ;
    // ========================================================================
    /** @var L0_TIMING
     *  Simple accessor to 'timing bit'
     * 
     *  @code 
     * 
     *   const LHCb::L0DUReport* l0 = ... ;
     * 
     *   bool timingBit = L0_TIMINGBIT ( l0 ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::L0::TimingBit
     *  @see LHCb::L0DUReport::timingTriggerBit
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    const LoKi::L0::TimingBit                                    L0_TIMINGBIT ;
    // ========================================================================
    /** @typedef L0_TRIGGER 
     *  Accessor to check the trigger decision 
     * 
     *  @code 
     * 
     *   const LHCb::L0DUReport* l0 = ... ;
     * 
     *   const std::vector<std::string>& triggers = ... ; 
     *
     *   const L0_TRIGGER t1 = L0_TRIGGER (  1             , 0 ) ;
     *   const L0_TRIGGER t2 = L0_TRIGGER ( 'trigger name' , 0 ) ;
     *   const L0_TRIGGER t3 = L0_TRIGGER (  triggers      , 0 ) ;
     * 
     *   const bool trig1 = t1 ( l0 ) ;
     *   const bool trig2 = t2 ( l0 ) ;
     *   const bool trig3 = t3 ( l0 ) ;
     *
     *  @endcode 
     *     
     *  @see LoKi::L0::TriggerDecision
     *  @see LHCb::L0DUReport::triggerDecision
     *  @see LHCb::L0DUReport::triggerDecisionByName  
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    typedef LoKi::L0::TriggerDecision                              L0_TRIGGER ;
    // ========================================================================
    /** @var L0_VALID 
     *  Tricial functor to check th wvalidity of L0DUReport object.
     *  Unlike all other L0-functors, is also accepts zero pointer.
     *  It acts according the rule:
     *  @code 
     *     return 0 != l0 && l0->valid() 
     *  @endcode 
     *   
     *  @code 
     *
     *    const LHCb::L0DUReport* l0 = ... ;
     *
     *    const bool valid = L0_VALID ( l0 ) ;
     *
     *  @code 
     *  
     *  @see LoKi::L0::Valid 
     *  @see LHCb::L0DUReport 
     *  @see LHCb::L0DUReport::valid 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-01-08
     */
    const LoKi::L0::Valid                                            L0_VALID ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_L0CUTS_H
// ============================================================================
