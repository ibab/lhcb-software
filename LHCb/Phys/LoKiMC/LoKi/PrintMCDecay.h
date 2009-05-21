// $Id: PrintMCDecay.h,v 1.10 2009-05-21 14:41:09 ibelyaev Exp $
// ===========================================================================
#ifndef LOKI_PRINTMCDECAY_H 
#define LOKI_PRINTMCDECAY_H 1
// ===========================================================================
// Include files
// ===========================================================================
// STD & STL 
// ===========================================================================
#include <string>
#include <limits>
#include <iostream>
// ===========================================================================
// LoKi
// ===========================================================================
#include "LoKi/MCTypes.h"
// ===========================================================================
// forward declarations 
// ===========================================================================
class MsgStream ;
// ===========================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2004-01-23 
 */
// ============================================================================
namespace LoKi 
{  
  // ==========================================================================
  namespace PrintMC 
  {
    // ========================================================================
    /// the maximal recursion level 
    const int s_maxLevel2 = 4 ;
    // ========================================================================
    /** Simple function to print MC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const LHCb::MCParticle* p = ... ;
     * 
     *  printDecay ( p , std::cout ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to MCParticle to be printed 
     *  @param stream   stream to be used 
     *  @param cut      condition 
     *  @param decayOnly consider only decay products 
     *  @param level the printout depth 
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    std::ostream&
    printDecay 
    ( const LHCb::MCParticle*      particle                , 
      std::ostream&                stream                  , 
      const LoKi::MCTypes::MCCuts& cut                     ,
      const bool                   decayOnly = true        ,
      const int                    level     = s_maxLevel2 ,
      const std::string&           blank     = "<cut>"     ) ;
    // ========================================================================
    /** Simple function to print MC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const LHCb::MCParticle* p = ... ;
     * 
     *  printDecay ( p , std::cout ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to MCParticle to be printed 
     *  @param stream   stream to be used  
     *  @param decayOnly consider only decay products 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    std::ostream&
    printDecay 
    ( const LHCb::MCParticle*      particle                , 
      std::ostream&                stream                  ,
      const bool                   decayOnly = true        ) ;
    // ========================================================================
    /** Simple function to print MC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const LHCb::MCParticle* p = ... ;
     * 
     *  printDecay( p , always() ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to MCParticle to be printed 
     *  @param stream   stream to be used 
     *  @param cut      condition 
     *  @param decayOnly consider only decay products 
     *  @param level the printout depth 
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    MsgStream& 
    printDecay 
    ( const LHCb::MCParticle*      particle                , 
      MsgStream&                   stream                  , 
      const LoKi::MCTypes::MCCuts& cut                     ,
      const bool                   decayOnly = true        ,
      const int                    level     = s_maxLevel2 ,
      const std::string&           blank     = "<cut>"     ) ;
    // ========================================================================
    /** Simple function to print MC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const LHCb::MCParticle* p = ... ;
     * 
     *  printDecay( p , always() ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to MCParticle to be printed 
     *  @param stream   stream to be used 
     *  @param decayOnly consider only decay products 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    MsgStream& 
    printDecay 
    ( const LHCb::MCParticle*      particle          , 
      MsgStream&                   stream            ,
      const bool                   decayOnly = true  ) ;
    // ========================================================================
    /** Simple function to print MC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const LHCb::MCParticle* p = ... ;
     * 
     *  info() << printDecay( p ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to MCParticle to be printed 
     *  @param cut      condition 
     *  @param decayOnly consider only decay products 
     *  @param level the printout depth 
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    std::string 
    printDecay 
    ( const LHCb::MCParticle*      particle                , 
      const LoKi::MCTypes::MCCuts& cut                     ,
      const bool                   decayOnly = true        ,
      const int                    level     = s_maxLevel2 ,
      const std::string&           blank     = "<cut>"     ) ;
    // ========================================================================
    /** Simple function to print MC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const LHCb::MCParticle* p = ... ;
     * 
     *  info() << printDecay( p ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to MCParticle to be printed 
     *  @param decayOnly consider only decay products 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    std::string 
    printDecay 
    ( const LHCb::MCParticle* particle          , 
      const bool              decayOnly = true  ) ;
    // ========================================================================
  } // end of namespace LoKi::PrintMC
  // ==========================================================================
  namespace Print
  {
    // ========================================================================
    using LoKi::PrintMC::s_maxLevel2 ;
    using namespace LoKi::PrintMC    ;
    // ========================================================================
  } // end of namespace LoKi::Print  
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PRINTMCDECAY_H
// ============================================================================
