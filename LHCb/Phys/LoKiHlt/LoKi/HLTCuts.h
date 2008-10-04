// $Id: HLTCuts.h,v 1.3 2008-10-04 11:48:55 ibelyaev Exp $
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
     *  simple functor which evaluates number of known selections  
     *  @see LoKi::HLT::Decision
     *  @see LHCb::HltDecReports 
     *  @see LHCb::HltDecReports::size
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-21
     */
    const LoKi::HLT::Decision                                    HLT_DECISION ;
    // ========================================================================
    /** @typedef HLT_HAS 
     *  Simple predicate to check the certain selections 
     *
     *  @code 
     * 
     *   const LHCb::HtlDecReports* hlt = ... ;
     * 
     *   std::vector<std::string>& selections = ... ; 
     * 
     *   const HLT_HAS h1 = HLT_HAS ( "some selection" ) ;
     *   const HLT_HAS h2 = HLT_HAS ( "selection1" ,"selection2" ) ; // 'OR'
     *   const HLT_HAS h3 = HLT_HAS (  selections ) ;                // 'OR'
     *
     *   const bool has1 = h1 ( hlt ) ;
     *   const bool has2 = h2 ( hlt ) ;
     *   const bool has3 = h3 ( hlt ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::HLT::HasSelection
     *  @see LHCb::HltDecReports 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-21
     */
    typedef LoKi::HLT::HasSelection                                   HLT_HAS ;
    // ========================================================================
    /** @var HLT_NPASS
     *  simple functor which evaluates the number of passed selections 
     *  @see LoKi::HLT::NPass
     *  @see LHCb::HltDecReports 
     *  @see LHCb::HltDecReport::decision
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-21
     */
    const LoKi::HLT::NPass                                          HLT_NPASS ;
    // ========================================================================
    /** @typedef HLT_PASS
     *  Simple predicate to check the positive decision forcertain selections
     *
     *  @code 
     * 
     *   const LHCb::HtlDecReports* hlt = ... ;
     * 
     *   std::vector<std::string>& selections = ... ; 
     * 
     *   const HLT_PASS p1 = HLT_PASS ( "some selection" ) ;
     *   const HLT_PASS p2 = HLT_PASS ( "selection1" ,"selection2" ) ; // 'OR'
     *   const HLT_PASS p3 = HLT_PASS (  selections ) ;                // 'OR'
     *
     *   const bool pass1 = p1 ( hlt ) ;
     *   const bool pass2 = p2 ( hlt ) ;
     *   const bool pass3 = p3 ( hlt ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::HLT::PassSelection
     *  @see LHCb::HltDecReports 
     *  @see LHCb::HltDecReport::decision
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-21
     */
    typedef LoKi::HLT::PassSelection                                 HLT_PASS ;
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
     *   const HLT_PASSIGNORING p1 = HLT_PASSIGNORING ( "some selection" ) ;
     *   const HLT_PASSIGNORING p2 = HLT_PASSIGNORING ( "selection1" ,"selection2" ) ; 
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
     *  simple functor which evaluates number of known selections  
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
