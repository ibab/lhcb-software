// $Id: PrintDecay.cpp,v 1.1 2006-05-27 11:44:03 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, verison $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
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
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/ParticleProperties.h"
#include "LoKi/Report.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PrintDecay.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for fuctions LoKi::printDecay 
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
 *  with the campain of <XXXXXXX> : 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-05-26 
 */
// ============================================================================

// ============================================================================
/// Simple function to print decay in more or less "readable" format 
// ============================================================================
MsgStream& LoKi::printDecay 
( const LHCb::Particle*        particle , 
  MsgStream&                   stream   , 
  const LoKi::Types::Cuts&     cut      ,
  const std::string&           blank    ) 
{
  if ( stream.isActive() ) 
  { LoKi::printDecay ( particle , stream.stream() , cut , blank ) ; }
  return stream ;
} ;
// ============================================================================
/// Simple function to print decay in more or less "readable" format 
// ============================================================================
std::string LoKi::printDecay 
( const LHCb::Particle*        particle , 
  const LoKi::Types::Cuts&     cut      ,
  const std::string&           blank    ) 
{
  std::ostringstream stream ;
  LoKi::printDecay ( particle , stream, cut , blank ) ;
  return stream.str() ;
} ;
// ============================================================================
/// Simple function to print decay in more or less "readable" format 
// ============================================================================
std::ostream& LoKi::printDecay 
( const LHCb::Particle*        particle , 
  std::ostream&                stream   , 
  const LoKi::Types::Cuts&     cut      ,
  const std::string&           blank    ) 
{
  if ( 0 == particle    ) 
  { 
    LoKi::Report::Warning ( "LoKi::printDecay, invalid particle" ) ; 
    return stream << " <NULL> " ;                                 // RETURN 
  }
  //
  if ( !cut( particle ) ) { return stream << blank ; } ;          // RETURN 
  //
  typedef SmartRefVector<LHCb::Particle> DAUGS  ;
  // own name 
  const std::string name = 
    LoKi::Particles::nameFromPID ( particle->particleID() ) ;
  const DAUGS& daugs = particle->daughters () ;
  if ( daugs.empty() ) { return stream << " " << name << " " ; } // RETURN 
  // print the decay 
  stream << " ( " << name << " -> " ;
  for ( DAUGS::const_iterator id = daugs.begin() ; daugs.end() != id ; ++id ) 
  { LoKi::printDecay ( *id , stream , cut , blank ) ; }          // RECURSION
  //  
  return stream << " ) " ;                                      // RETURN 
} ;
// ============================================================================


// ============================================================================
// The END 
// ============================================================================

