// ============================================================================
#ifndef LOKI_ALGCUTS_H 
#define LOKI_ALGCUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/AlgFunctors.h"
// ============================================================================
/** @file LoKi/AlgCuts.h
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-17
 */
namespace LoKi 
{
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef ALG_ALLENABLED
     *  simple functor to check if all algorithms from thelist are 'enabled'
     *
     *  @code 
     *
     *   const ALG_ALLENABLED enabled ( "StdTightD02KK", "..." ) ;
     *
     *   const bool ok = enabled () ;
     *
     *  @endcode 
     *
     *  @see Algorithm:isEnabled 
     *  @todo ALG_* stuff need to be recoded as soon as 
     *        IAlgorithm::isEnabled appears
     *  @see LoKi::Algorithms::AllEnabled 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-19
     */
    typedef LoKi::Algorithms::AllEnabled                       ALG_ALLENABLED ;
    // ========================================================================
    /** @typedef ALG_ALLEXECUTED
     *  simple functor to check if all algorithms from the list are 'executed'
     *
     *  @code 
     *
     *   const ALG_ALLEXECUTED executed ( "StdTightD02KK" , "..." , "...") ;
     *
     *   const bool ok = executed () ;
     *
     *  @endcode 
     *
     *  @see  IAlgorithm:isExecuted
     *  @see LoKi::Algorithms::AllExecuted
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-19
     */
    typedef LoKi::Algorithms::AllExecuted                     ALG_ALLEXECUTED ;
    // ========================================================================
    /** @typedef ALG_ALLPASSED 
     *  simple functor to check if all algorithms from the list passed filter 
     *
     *  @code 
     *
     *   const ALG_ALLPASSED passed ( "StdTightD02KK", "..", "..." ) ;
     *
     *   const bool filtered = passed () ;
     *
     *  @endcode 
     *
     *  @see Algorithm:filterPassed 
     *  @todo ALG_* stuff need to be recoded as soon as 
     *        IAlgorithm::filterPassed appears
     *  @see LoKi::Algorithms::AllPassed 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-19
     */
    typedef LoKi::Algorithms::AllPassed                         ALG_LLPASSED ;
    // ========================================================================
    /** @typedef ALG_ANYENABLED
     *  simple functor to check if any algorithm from thelist is 'enabled'
     *
     *  @code 
     *
     *   const ALG_ANYENABLED enabled ( "StdTightD02KK", "..." ) ;
     *
     *   const bool ok = enabled () ;
     *
     *  @endcode 
     *
     *  @see Algorithm:isEnabled 
     *  @todo ALG_* stuff need to be recoded as soon as 
     *        IAlgorithm::isEnabled appears
     *  @see LoKi::Algorithms::AnyEnabled 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-19
     */
    typedef LoKi::Algorithms::AnyEnabled                       ALG_ANYENABLED ;
    // ========================================================================
    /** @typedef ALG_ANYEXECUTED
     *  simple functor to check if any algorithm from the list is 'executed'
     *
     *  @code 
     *
     *   const ALG_ANYEXECUTED executed ( "StdTightD02KK" , "..." , "...") ;
     *
     *   const bool ok = executed () ;
     *
     *  @endcode 
     *
     *  @see  IAlgorithm:isExecuted
     *  @see LoKi::Algorithms::AnyExecuted
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-19
     */
    typedef LoKi::Algorithms::AnyExecuted                     ALG_ANYEXECUTED ;
    // ========================================================================
    /** @typedef ALG_ANYPASSED 
     *  simple functor to check if any algorithm from the list passed filter 
     *
     *  @code 
     *
     *   const ALG_ANYPASSED passed ( "StdTightD02KK", "..", "..." ) ;
     *
     *   const bool filtered = passed () ;
     *
     *  @endcode 
     *
     *  @see Algorithm:filterPassed 
     *  @todo ALG_* stuff need to be recoded as soon as 
     *        IAlgorithm::filterPassed appears
     *  @see LoKi::Algorithms::AnyPassed 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-19
     */
    typedef LoKi::Algorithms::AnyPassed                         ALG_ANYPASSED ;
    // ========================================================================
    /** @typedef ALG_ENABLED
     *  simple functor to check if given algorithm 'enabled'
     *
     *  @code 
     *
     *   const ALG_ENABLED enabled ( "StdTightD02KK" ) ;
     *
     *   const bool ok = enabled () ;
     *
     *  @endcode 
     *
     *  @see Algorithm:isEnabled 
     *  @todo ALG_* stuff need to be recoded as soon as 
     *        IAlgorithm::isEnabled appears
     *  @see LoKi::Algorithms::Enabled 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-19
     */
    typedef LoKi::Algorithms::Enabled                             ALG_ENABLED ;
    // ========================================================================
    /** @typedef ALG_EXECUTED
     *  simple functor to check if given algorithm 'executed'
     *
     *  @code 
     *
     *   const ALG_EXECUTED executed ( "StdTightD02KK" ) ;
     *
     *   const bool ok = executed () ;
     *
     *  @endcode 
     *
     *  @see  IAlgorithm:isExecuted
     *  @see LoKi::Algorithms::Executed
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-19
     */
    typedef LoKi::Algorithms::Executed                           ALG_EXECUTED ;
    // ========================================================================
    /** @typedef ALG_PASSED 
     *  simple functor to check if given algorithm passed filter 
     *
     *  @code 
     *
     *   const ALG_PASSED passed ( "StdTightD02KK" ) ;
     *
     *   const bool filtered = passed () ;
     *
     *  @endcode 
     *
     *  @see Algorithm:filterPassed 
     *  @todo ALG_* stuff need to be recoded as soon as 
     *        IAlgorithm::filterPassed appears
     *  @see LoKi::Algorithms::Passed 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-19
     */
    typedef LoKi::Algorithms::Passed                               ALG_PASSED ;
    // ========================================================================
    /** @typedef ALG_NUMENABLED
     *  simple functor to ount algorithms from the list are 'enabled'
     *
     *  @code 
     *
     *   const ALG_NUMENABLED nenabled ( "StdTightD02KK", "..." ) ;
     *
     *   const bool ok = 6 < nenabled () ;
     *
     *  @endcode 
     *
     *  @see Algorithm:isEnabled 
     *  @todo ALG_* stuff need to be recoded as soon as 
     *        IAlgorithm::isEnabled appears
     *  @see LoKi::Algorithms::AllEnabled 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-19
     */
    typedef LoKi::Algorithms::AllEnabled                       ALG_ALLENABLED ;
    // ========================================================================
    /** @typedef ALG_NUMEXECUTED
     *  simple functor to count algorithms from the list are 'executed'
     *
     *  @code 
     *
     *   const ALG_NUMEXECUTED nexecuted ( "StdTightD02KK" , "..." , "...") ;
     *
     *   const bool ok = 5 < nexecuted () ;
     *
     *  @endcode 
     *
     *  @see  IAlgorithm:isExecuted
     *  @see LoKi::Algorithms::NumExecuted
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-19
     */
    typedef LoKi::Algorithms::NumExecuted                     ALG_NUMEXECUTED ;
    // ========================================================================
    /** @typedef ALG_NUMPASSED 
     *  simple functor to count algorithms from the list passed filter 
     *
     *  @code 
     *
     *   const ALG_ALLPASSED npassed ( "StdTightD02KK", "..", "..." ) ;
     *
     *   const bool OK = npassed() > 3 ;
     *
     *  @endcode 
     *
     *  @see Algorithm:filterPassed 
     *  @todo ALG_* stuff need to be recoded as soon as 
     *        IAlgorithm::filterPassed appears
     *  @see LoKi::Algorithms::NumPassed 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-19
     */
    typedef LoKi::Algorithms::NumPassed                         ALG_NUMPASSED ;
    // ========================================================================
  } // end of namespace LoKi::Cuts 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ALGCUTS_H
// ============================================================================
