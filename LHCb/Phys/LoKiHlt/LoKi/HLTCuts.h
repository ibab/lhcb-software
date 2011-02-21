// $Id: HLTCuts.h,v 1.6 2010-05-17 16:01:38 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_HLTCUTS_H 
#define LOKI_HLTCUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/HLT.h"
// ============================================================================
/** @file 
 *  Collection of HLT-related functors 
 *  @author Vanay BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-09-21
 */
namespace LoKi 
{
  // ==========================================================================
  namespace Cuts 
  {
    // Suppress Intel compiler warnings about missing default constructor
    // In this case the compiler generated constructor is fine, since there are
    // no member data to be initialised
    #ifdef __INTEL_COMPILER
      #pragma warning(disable:854)
      #pragma warning(push)
    #endif
    // ========================================================================
    /** @typedef HLT_COUNT_ERRORBITS
     *  simple functor to get the error bits for some lines 
     *  @code 
     * 
     *   const LHCb::HtlDecReports* hlt = ... ;
     * 
     *   const HLT_ERRORBITS h1 = HLT_ERRORBITS  ( "some decision" ) ;
     *
     *   const bool ok = 0 != h1 ( hlt ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::HLT::ErrorBits 
     *  @see LHCb::HltDecReport::errorBits
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-17
     */
    typedef LoKi::HLT::ErrorBits                                HLT_ERRORBITS ;
    // ========================================================================
    /** @var HLT_DECISION
     *  simple functor which evaluates number of known decisions  
     *  @see LoKi::HLT::Decision
     *  @see LHCb::HltDecReports 
     *  @see LHCb::HltDecReports::size
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-21
     */
    const LoKi::HLT::Decision                                    HLT_DECISION ;
    // ========================================================================
    /** @typedef HLT_ERRORBITS 
     *  simple functor to get the error bits for the certain line 
     *  @code 
     * 
     *   const LHCb::HtlDecReports* hlt = ... ;
     * 
     *   const HLT_ERRORBITS h1 = HLT_ERRORBITS  ( "some decision" ) ;
     *
     *   const bool ok = 0 != h1 ( hlt ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::HLT::ErrorBits 
     *  @see LHCb::HltDecReport::errorBits
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-17
     */
    typedef LoKi::HLT::ErrorBits                                HLT_ERRORBITS ;
    // ========================================================================
    /** @typedef HLT_EXECUTIONSTAGE
     *  simple functor to get the error bits for the certain line 
     *  @code 
     * 
     *   const LHCb::HtlDecReports* hlt = ... ;
     * 
     *   const HLT_EXECUTIONSTAGE h = HLT_EXECUTIONSTAGE ( "some decision" ) ;
     *
     *   const bool ok = 6 <  h ( hlt ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::HLT::ExcecutionStage
     *  @see LHCb::HltDecReport::executionStage 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-17
     */
    typedef LoKi::HLT::ExecutionStage                      HLT_EXECUTIONSTAGE ;
    // ========================================================================
    /** @typedef HLT_HAS 
     *  Simple predicate to check the certain decisions 
     *
     *  @code 
     * 
     *   const LHCb::HtlDecReports* hlt = ... ;
     * 
     *   std::vector<std::string>& decisions = ... ; 
     * 
     *   const HLT_HAS h1 = HLT_HAS ( "some decision" ) ;
     *   const HLT_HAS h2 = HLT_HAS ( "decision1" ,"decision2" ) ; // 'OR'
     *   const HLT_HAS h3 = HLT_HAS (  decisions ) ;                // 'OR'
     *
     *   const bool has1 = h1 ( hlt ) ;
     *   const bool has2 = h2 ( hlt ) ;
     *   const bool has3 = h3 ( hlt ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::HLT::HasDecision
     *  @see LHCb::HltDecReports 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-21
     */
    typedef LoKi::HLT::HasDecision                                   HLT_HAS ;
    // ========================================================================
    /** @typedef HLT_HAS_SUBSTR 
     *  Simple predicate to check the presence of decison with has 
     *  a certain substring
     *
     *  @code 
     * 
     *   const LHCb::HtlDecReports* hlt = ... ;
     * 
     *   const HLT_HAS_SUBST h1 = HLT_HAS ( "Muon" ) ;
     *
     *   const bool has1 = h1 ( hlt ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::HLT::HasDecisionSubString
     *  @see LHCb::HltDecReports 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-21
     */
    typedef LoKi::HLT::HasDecisionSubString                    HLT_HAS_SUBSTR ;
    // ========================================================================
    /** @typedef HLT_HAS_RE
     *  Simple predicate to check the presence of decison with has 
     *  match with regular expression 
     *
     *  @code 
     * 
     *   const LHCb::HtlDecReports* hlt = ... ;
     * 
     *   const HLT_HAS_RE h1 = HLT_HAS_RE ( *some regular expression here* ) ;
     *
     *   const bool has1 = h1 ( hlt ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::HLT::HasDecisionRegex
     *  @see LHCb::HltDecReports 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-21
     */
    typedef LoKi::HLT::HasDecisionRegex                            HLT_HAS_RE ;
    // ========================================================================
    /** @var HLT_NPASS
     *  simple functor which evaluates the number of passed decision 
     *  @see LoKi::HLT::NPass
     *  @see LHCb::HltDecReports 
     *  @see LHCb::HltDecReport::decision
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-21
     */
    const LoKi::HLT::NPass                                          HLT_NPASS ;
    // ========================================================================
    /** @typedef HLT_NCANDIDATES 
     *  simple functor to get the number of candidates 
     *  @code 
     * 
     *   const LHCb::HtlDecReports* hlt = ... ;
     * 
     *   const HLT_NCANDIDATES h = HLT_CANDIDATES ( "some decision" ) ;
     *
     *   const bool ok = 10 >  h ( hlt ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::HLT::NumberOfCandidated 
     *  @see LHCb::HltDecReport::numberOfCandidates
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-17
     */
    typedef LoKi::HLT::NumberOfCandidates                     HLT_NCANDIDATES ;
    // ========================================================================
    /** @typedef HLT_ROUTINGBITS 
     *  Simple functor tocheck the routing bits 
     *  @code
     * 
     *   const LHCb::HtlDecReports* hlt = ... ;
     * 
     *  const HLT_ROUTINGBITS bits = HLT_ROUTINGBITS ( ROUTINGBITS ( 1, 2, 3  ) ) ;
     *
     *  const bool ok = bits ( odin ) ;
     *
     *  @endcode 
     *
     *  @see LHCb::HLT::HltRoutingBits 
     *  @see LHCb::HLT::RoutingBits 
     *  @see LHCb::Cuts::ROUTINGBITS 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-17     
     */
    typedef LoKi::HLT::HltRoutingBits                         HLT_ROUTINGBITS ;
    // ========================================================================
    /** @typedef HLT_PASS
     *  Simple predicate to check the positive decision forcertain decision
     *
     *  @code 
     * 
     *   const LHCb::HtlDecReports* hlt = ... ;
     * 
     *   std::vector<std::string>& decisions = ... ; 
     * 
     *   const HLT_PASS p1 = HLT_PASS ( "some decision" ) ;
     *   const HLT_PASS p2 = HLT_PASS ( "decision1" ,"decision2" ) ; // 'OR'
     *   const HLT_PASS p3 = HLT_PASS (  decisions ) ;                // 'OR'
     *
     *   const bool pass1 = p1 ( hlt ) ;
     *   const bool pass2 = p2 ( hlt ) ;
     *   const bool pass3 = p3 ( hlt ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::HLT::PassDecision
     *  @see LHCb::HltDecReports 
     *  @see LHCb::HltDecReport::decision
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-21
     */
    typedef LoKi::HLT::PassDecision                                 HLT_PASS ;
    // ========================================================================
    /** @typedef HLT_PASS_SUBSTR
     *  Simple predicate to check the positive decision for the selection
     *  which matches the substring 
     *
     *  @code 
     * 
     *   const LHCb::HtlDecReports* hlt = ... ;
     * 
     *   const HLT_PASS_SUBSTR p1 = HLT_PASS_SUBSTR ( "Muon" ) ;
     *
     *   const bool pass1 = p1 ( hlt ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::HLT::PassDecisionSubString
     *  @see LHCb::HltDecReports 
     *  @see LHCb::HltDecReport::decision
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-21
     */
    typedef LoKi::HLT::PassDecisionSubString                  HLT_PASS_SUBSTR ;
    // ========================================================================
    /** @typedef HLT_PASS_RE
     *  Simple predicate to check the positive decision for the selection
     *  which matches the substring 
     *
     *  @code 
     * 
     *   const LHCb::HtlDecReports* hlt = ... ;
     * 
     *   const HLT_PASS_RE p1 = HLT_PASS_RE ( "some regular expression" ) ;
     *
     *   const bool pass1 = p1 ( hlt ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::HLT::PassDecisionRegex
     *  @see LHCb::HltDecReports 
     *  @see LHCb::HltDecReport::decision
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-21
     */
    typedef LoKi::HLT::PassDecisionRegex                          HLT_PASS_RE ;
    // ========================================================================
    /** @typedef HLT_PASSIGNORING 
     *  simple predicate which checks the overall decision, 
     *  ignoring "the special" decisions:
     *
     *  @code 
     *
     *   const LHCb::HtlDecReports* hlt = ... ;
     * 
     *   std::vector<std::string>& special = ... ; 
     * 
     *   const HLT_PASSIGNORING p1 = HLT_PASSIGNORING ( "some decision" ) ;
     *   const HLT_PASSIGNORING p2 = HLT_PASSIGNORING ( "decision1" ,
     *                                                  "decision2" ) ;
     *   const HLT_PASSIGNORING p3 = HLT_PASSIGNORING (  special ) ;              
     *
     *   const bool pass1 = p1 ( hlt ) ;
     *   const bool pass2 = p2 ( hlt ) ;
     *   const bool pass3 = p3 ( hlt ) ;
     *
     *  @endcode 
     *  @see LoKi::HLT::DecisionBut 
     *  @see LHCb::HltDecReports 
     *  @see LHCb::HltDecReport::decision
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-03
     */
    typedef LoKi::HLT::DecisionBut                           HLT_PASSIGNORING ;
    // ========================================================================
    /** @typedef HLT_PASSIGNORING_SUBSTR 
     *  simple predicate which checks the overall decision, 
     *  ignoring "the special" decisions:
     *
     *  @code 
     *
     *   const LHCb::HtlDecReports* hlt = ... ;
     * 
     *   const HLT_PASSIGNORING_SUBSTR p1 = HLT_PASSIGNORING_SUBSTR ( "Lumi" ) ;
     *
     *   const bool pass1 = p1 ( hlt ) ;
     *
     *  @endcode 
     *  @see LoKi::HLT::DecisionButSubString 
     *  @see LHCb::HltDecReports 
     *  @see LHCb::HltDecReport::decision
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-03
     */
    typedef LoKi::HLT::DecisionButSubString           HLT_PASSIGNORING_SUBSTR ;
    // ========================================================================
    /** @typedef HLT_PASSIGNORING_RE
     *  simple predicate which checks the overall decision, 
     *  ignoring "the special" decisions:
     *
     *  @code 
     *
     *   const LHCb::HtlDecReports* hlt = ... ;
     * 
     *   const HLT_PASSIGNORING_RE p1 = HLT_PASSIGNORING_RE ( "Lumi" ) ;
     *
     *   const bool pass1 = p1 ( hlt ) ;
     *
     *  @endcode 
     *  @see LoKi::HLT::DecisionButRegex
     *  @see LHCb::HltDecReports 
     *  @see LHCb::HltDecReport::decision
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-03
     */
    typedef LoKi::HLT::DecisionButRegex                   HLT_PASSIGNORING_RE ;
    // ========================================================================
    /** @typedef HLT_SATURATED
     *  simple functor to check if number fo candidates is saturated 
     *  @code 
     * 
     *   const LHCb::HtlDecReports* hlt = ... ;
     * 
     *   const HLT_SATURATED h = HLT_SATURATED ( "some decision" ) ;
     *
     *   const bool ok = h ( hlt ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::HLT::Saturated
     *  @see LoKi::HLT::NumberOfCandidated 
     *  @see LHCb::HltDecReport::numberOfCandidates
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-17
     */
    typedef LoKi::HLT::Saturated                                HLT_SATURATED ;
    // ========================================================================
    /** @var HLT_SIZE 
     *  simple functor which evaluates number of known decisions  
     *  @see LoKi::HLT::Size
     *  @see LHCb::HltDecReports 
     *  @see LHCb::HltDecReports::size
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-21
     */
    const LoKi::HLT::Size                                            HLT_SIZE ;

    #ifdef __INTEL_COMPILER
      #pragma warning(pop) // End disable ICC warning #854
    #endif
    // ========================================================================
  } // end of namespace LoKi::Cuts 
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HLTCUTS_H
// ============================================================================
