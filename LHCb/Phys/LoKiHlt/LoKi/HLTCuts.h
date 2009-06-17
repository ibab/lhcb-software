// $Id: HLTCuts.h,v 1.5 2009-06-17 12:02:57 ibelyaev Exp $
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
     *   const HLT_PASSIGNORING p2 = HLT_PASSIGNORING ( "decision1" ,"decision2" ) ; 
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
    /** @var HLT_SIZE 
     *  simple functor which evaluates number of known decisions  
     *  @see LoKi::HLT::Size
     *  @see LHCb::HltDecReports 
     *  @see LHCb::HltDecReports::size
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-21
     */
    const LoKi::HLT::Size                                            HLT_SIZE ;
    // ========================================================================
  } // end of namespace LoKi::Cuts 
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HLTCUTS_H
// ============================================================================
