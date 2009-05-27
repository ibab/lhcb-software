// $Id: HLTCuts.h,v 1.4 2009-05-27 06:40:02 graven Exp $
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
