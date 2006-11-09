// $Id: PrintMCDecay.h,v 1.4 2006-11-09 16:35:00 ibelyaev Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/05/27 11:40:51  ibelyaev
//  add PrintMCDecay utilities
//
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
// MCEvent
// ===========================================================================
#include "Event/MCParticle.h"
// ===========================================================================
// LoKi
// ===========================================================================
#include "LoKi/MCTypes.h"
// ===========================================================================
// forward declarations 
// ===========================================================================
class MsgStream ;
// ===========================================================================

// ============================================================================
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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of <XXXXXXX> et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2004-01-23 
 */
// ============================================================================

namespace LoKi 
{  
  namespace Print 
  {
    /// maximal possible recursion level 
    const int s_maxLevel2 = std::numeric_limits<int>::max() ;
    
    /** @fn printMCDecay
     *  Simple function to print MC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const LHCb::MCParticle* p = ... ;
     * 
     *  printMCDecay( p , std::cout ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to MCParticle to be printed 
     *  @param stream   stream to be used 
     *  @param cut      condition 
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    std::ostream& printMCDecay 
    ( const LHCb::MCParticle*      particle                    , 
      std::ostream&                stream                      , 
      const LoKi::MCTypes::MCCuts& cut   = 
      LoKi::BooleanConstant<const  LHCb::MCParticle*> ( true ) , 
      const int                    level = s_maxLevel2         ,
      const std::string&           blank = "<cut>"             ) ;
    /** @fn printMCDecay
     *  Simple function to print MC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const LHCb::MCParticle* p = ... ;
     * 
     *  printMCDecay( p , always() ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to MCParticle to be printed 
     *  @param stream   stream to be used 
     *  @param cut      condition 
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    MsgStream& printMCDecay 
    ( const LHCb::MCParticle*      particle                    , 
      MsgStream&                   stream                      , 
      const LoKi::MCTypes::MCCuts& cut   = 
      LoKi::BooleanConstant<const  LHCb::MCParticle*> ( true ) , 
      const int                    level = s_maxLevel2         ,
      const std::string&           blank = "<cut>"             ) ;
    /** @fn printMCDecay
     *  Simple function to print MC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const LHCb::MCParticle* p = ... ;
     * 
     *  info() << printMCDecay( p ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to MCParticle to be printed 
     *  @param cut      condition 
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    std::string printMCDecay 
    ( const LHCb::MCParticle*      particle                    , 
      const LoKi::MCTypes::MCCuts& cut   = 
      LoKi::BooleanConstant<const  LHCb::MCParticle*> ( true ) , 
      const int                    level = s_maxLevel2         ,
      const std::string&           blank = "<cut>"             ) ;
    /** @fn printDecay
     *  Simple function to print MC decay in more or less "readable" format 
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
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    inline std::string printDecay 
    ( const LHCb::MCParticle*      particle                    , 
      const LoKi::MCTypes::MCCuts& cut   = 
      LoKi::BooleanConstant<const  LHCb::MCParticle*> ( true ) , 
      const int                    level = s_maxLevel2         ,
      const std::string&           blank = "<cut>"             )
    { return printMCDecay ( particle , cut , level , blank ) ; }
    /** @fn printDecay
     *  Simple function to print MC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const LHCb::MCParticle* p = ... ;
     * 
     *  printDecay( p , std::cout ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to MCParticle to be printed 
     *  @param stream   stream to be used 
     *  @param cut      condition 
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    inline std::ostream& printDecay 
    ( const LHCb::MCParticle*      particle                    , 
      std::ostream&                stream                      , 
      const LoKi::MCTypes::MCCuts& cut   = 
      LoKi::BooleanConstant<const  LHCb::MCParticle*> ( true ) , 
      const int                    level = s_maxLevel2         ,
      const std::string&           blank = "<cut>"             ) 
    { return printMCDecay ( particle , stream , cut , level , blank ) ; }
    /** @fn printDecay
     *  Simple function to print MC decay in more or less "readable" format 
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
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    inline MsgStream& printDecay 
    ( const LHCb::MCParticle*      particle                    , 
      MsgStream&                   stream                      , 
      const LoKi::MCTypes::MCCuts& cut   = 
      LoKi::BooleanConstant<const  LHCb::MCParticle*> ( true ) , 
      const int                    level = s_maxLevel2         ,
      const std::string&           blank = "<cut>"             ) 
    { return printMCDecay ( particle , stream , cut , level , blank ) ; }

  } // end of namespace Print  
}  // end of namespace LoKi 

// ===========================================================================
// The END 
// ===========================================================================
#endif // LOKI_PRINTMCDECAY_H
// ===========================================================================
