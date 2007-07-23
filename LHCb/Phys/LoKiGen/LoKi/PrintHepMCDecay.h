// $Id: PrintHepMCDecay.h,v 1.4 2007-07-23 17:23:36 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PRINTHEPMCDECAY_H 
#define LOKI_PRINTHEPMCDECAY_H 1
// ============================================================================
// Include files
// ===========================================================================
// STD & STL 
// ===========================================================================
#include <string>
#include <iostream>
#include <limits>
// ===========================================================================
// HepMC
// ===========================================================================
#include "HepMC/GenParticle.h"
// ===========================================================================
// LoKi
// ===========================================================================
#include "LoKi/GenTypes.h"
// ===========================================================================
// forward declarations 
// ===========================================================================
class MsgStream ;
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
 *  with the campain of \<XXXXXXX\> et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-05-26 
 */
// ============================================================================
namespace LoKi 
{  
  namespace Print 
  {
    // ============================================================================
    /// the maximal recursion level 
    const int s_maxLevel1 = std::numeric_limits<int>::max() ;
    // ============================================================================
    /** Simple function to print HepMC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const HepMC::GenParticle* p = ... ;
     * 
     *  printHepMCDecay( p , std::cout ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to HepMC::GenParticle to be printed 
     *  @param stream   stream to be used 
     *  @param cut      condition 
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    std::ostream& 
    printHepMCDecay 
    ( const HepMC::GenParticle*      particle            , 
      std::ostream&                  stream              , 
      const LoKi::GenTypes::GCuts&   cut                 ,
      const int                      level = s_maxLevel1 ,
      const std::string&             blank = "<cut>"     ) ;
    // ============================================================================
    /** Simple function to print HepMC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const HepMC::GenParticle* p = ... ;
     * 
     *  printHepMCDecay( p , std::cout ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to HepMC::GenParticle to be printed 
     *  @param stream   stream to be used 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    std::ostream& 
    printHepMCDecay 
    ( const HepMC::GenParticle*      particle  , 
      std::ostream&                  stream    ) ;
    // ============================================================================
    /** Simple function to print HepMC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const HepMC::GenParticle* p = ... ;
     * 
     *  printHepMCDecay( p , always() ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to HepMC::GenParticle to be printed 
     *  @param stream   stream to be used 
     *  @param cut      condition 
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    MsgStream& 
    printHepMCDecay   
    ( const HepMC::GenParticle*    particle            , 
      MsgStream&                   stream              , 
      const LoKi::GenTypes::GCuts& cut                 ,
      const int                    level = s_maxLevel1 ,
      const std::string&           blank = "<cut>"     ) ;
    // ============================================================================
    /** Simple function to print HepMC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const HepMC::GenParticle* p = ... ;
     * 
     *  printHepMCDecay( p , always() ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to HepMC::GenParticle to be printed 
     *  @param stream   stream to be used 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    MsgStream& 
    printHepMCDecay 
    ( const HepMC::GenParticle*    particle  , 
      MsgStream&                   stream    ) ;
    // ============================================================================
    /** Simple function to print HepMC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const HepMC::GenParticle* p = ... ;
     * 
     *  info() << printHepMCDecay( p ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to HepMC::GenParticle to be printed 
     *  @param cut      condition 
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    std::string 
    printHepMCDecay 
    ( const HepMC::GenParticle*    particle            , 
      const LoKi::GenTypes::GCuts& cut                 , 
      const int                    level = s_maxLevel1 ,
      const std::string&           blank = "<cut>"     ) ;
    // ============================================================================
    /** Simple function to print HepMC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const HepMC::GenParticle* p = ... ;
     * 
     *  info() << printHepMCDecay( p ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to HepMC::GenParticle to be printed 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    std::string 
    printHepMCDecay 
    ( const HepMC::GenParticle*    particle ) ;
    // ============================================================================
    /** Simple function to print HepMC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const HepMC::GenParticle* p = ... ;
     * 
     *  info() << printDecay( p ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to HepMC::GenParticle to be printed 
     *  @param cut      condition 
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    inline std::string 
    printDecay 
    ( const HepMC::GenParticle*    particle            , 
      const LoKi::GenTypes::GCuts& cut                 ,
      const int                    level = s_maxLevel1 ,
      const std::string&           blank = "<cut>"     )
    { return printHepMCDecay ( particle , cut , level , blank ) ; }
    // ============================================================================
    /** Simple function to print HepMC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const HepMC::GenParticle* p = ... ;
     * 
     *  info() << printDecay( p ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to HepMC::GenParticle to be printed 
     *  @param cut      condition 
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    inline std::string 
    printDecay 
    ( const HepMC::GenParticle*    particle ) 
    { return printHepMCDecay ( particle ) ; }
    // ============================================================================
    /** Simple function to print HepMC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const HepMC::GenParticle* p = ... ;
     * 
     *  printDecay( p , std::cout ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to HepMC::GenParticle to be printed 
     *  @param stream   stream to be used 
     *  @param cut      condition 
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    inline std::ostream& 
    printDecay 
    ( const HepMC::GenParticle*      particle            , 
      std::ostream&                  stream              , 
      const LoKi::GenTypes::GCuts&   cut                 ,
      const int                      level = s_maxLevel1 ,
      const std::string&             blank = "<cut>"     ) 
    { return printHepMCDecay ( particle , stream , cut , level , blank ) ; }
    // ============================================================================
    /** Simple function to print HepMC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const HepMC::GenParticle* p = ... ;
     * 
     *  printDecay( p , std::cout ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to HepMC::GenParticle to be printed 
     *  @param stream   stream to be used 
     *  @param cut      condition 
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    inline std::ostream& 
    printDecay 
    ( const HepMC::GenParticle*      particle  , 
      std::ostream&                  stream    ) 
    { return printHepMCDecay ( particle , stream ) ; }
    // ============================================================================
    /** Simple function to print HepMC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const HepMC::GenParticle* p = ... ;
     * 
     *  printDecay( p , always() ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to HepMC::GenParticle to be printed 
     *  @param stream   stream to be used 
     *  @param cut      condition 
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    inline MsgStream& 
    printDecay 
    ( const HepMC::GenParticle*    particle                     , 
      MsgStream&                   stream                       , 
      const LoKi::GenTypes::GCuts& cut   ,
      const int                    level = s_maxLevel1          ,
      const std::string&           blank = "<cut>"              ) 
    { return printHepMCDecay ( particle , stream , cut , level , blank ) ; }
    // ============================================================================
    /** Simple function to print HepMC decay in more or less "readable" format 
     *
     *  @code
     *
     *  const HepMC::GenParticle* p = ... ;
     * 
     *  printDecay( p , always() ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to HepMC::GenParticle to be printed 
     *  @param stream   stream to be used 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    inline MsgStream& 
    printDecay 
    ( const HepMC::GenParticle*    particle  , 
      MsgStream&                   stream    )
    { return printHepMCDecay ( particle , stream ) ; }
    // ============================================================================    
  } // end of namespace Print  
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PRINTHEPMCDECAY_H
// ============================================================================
