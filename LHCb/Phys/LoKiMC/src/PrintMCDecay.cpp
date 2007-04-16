// $Id: PrintMCDecay.cpp,v 1.7 2007-04-16 16:16:08 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2006/11/27 11:58:37  ibelyaev
//  prepare for LoKi v4r3
//
// Revision 1.5  2006/11/09 16:35:01  ibelyaev
//  v1r4: improve the printout functions
//
// Revision 1.4  2006/05/27 11:40:51  ibelyaev
//  add PrintMCDecay utilities
//
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
#include <sstream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// MCEvent
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ParticleProperties.h"
#include "LoKi/PrintMCDecay.h"
#include "LoKi/MCExtract.h"
#include "LoKi/Report.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for the function printMCDecay
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
 *  @date 2001-01-23 
 */
// ============================================================================

// ============================================================================
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
// ============================================================================
MsgStream& LoKi::Print::printMCDecay 
( const LHCb::MCParticle*      particle , 
  MsgStream&                   stream   , 
  const LoKi::MCTypes::MCCuts& cut      , 
  const int                    level    , 
  const std::string&           blank    ) 
{
  if ( stream.isActive() ) 
  { LoKi::Print::printMCDecay 
      ( particle , stream.stream() , cut , level , blank ) ; }
  return stream ;
};
// ============================================================================
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
// ============================================================================
std::string LoKi::Print::printMCDecay 
( const LHCb::MCParticle*      particle  , 
  const LoKi::MCTypes::MCCuts& cut       , 
  const int                    level     , 
  const std::string&           blank     ) 
{
  std::ostringstream stream ;
  LoKi::Print::printMCDecay ( particle , stream , cut , level , blank ) ;
  return stream.str() ;
} ;
// ============================================================================
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
// ============================================================================
std::ostream& LoKi::Print::printMCDecay 
( const LHCb::MCParticle*      particle , 
  std::ostream&                stream   , 
  const LoKi::MCTypes::MCCuts& cut      ,
  const int                    level    , 
  const std::string&           blank    ) 
{
  if ( 0 == particle    ) 
  { 
    LoKi::Report::Warning ( "LoKi::printMCDecay, invalid particle" ) ; 
    return stream << " <NULL> " ;                           // RETURN 
  }
  //
  if ( !cut( particle ) ) { return stream << blank ; } ;    // RETURN 
  //
  typedef std::vector<const LHCb::MCParticle*> MCPs ;
  MCPs daugs ;
  LoKi::Extract::getMCChildren 
    ( particle , std::back_inserter( daugs ) ) ;
  // own name 
  const std::string name = 
    LoKi::Particles::nameFromPID ( particle->particleID() ) ;
  if ( daugs.empty() ) 
  { return stream << " " << name << " " ; } ;               // RETURN 
  // 
  if ( 0 >= level    ) { return stream << " " << name << " " ; } ; // RETURN 
  // print the decay 
  stream << " ( " << name << " ->" ;
  for ( MCPs::const_iterator id = daugs.begin() ; 
        daugs.end() != id ; ++id ) 
  { LoKi::Print::printMCDecay 
      ( *id , stream , cut , level - 1 , blank ) ; }   // RECURSION
  //
  return stream << " ) " ;
} ;
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
