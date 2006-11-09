// $Id: PrintHepMCDecay.cpp,v 1.2 2006-11-09 16:35:32 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/05/27 11:38:44  ibelyaev
//  add PrintHepMCDecay utilities
// 
// ============================================================================
// Include files 
// ============================================================================
// Include files
// ===========================================================================
// STD & STL 
// ===========================================================================
#include <string>
#include <iostream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/MsgStream.h"
// ============================================================================
// Event
// ===========================================================================
#include "Event/HepMCEvent.h"
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/ParticleProperties.h"
#include "LoKi/Report.h"
#include "LoKi/Print.h"
// ===========================================================================
// LoKiGen
// ===========================================================================
#include "LoKi/GenTypes.h"
// ===========================================================================
// Local 
// ===========================================================================
#include "LoKi/PrintHepMCDecay.h"
// ===========================================================================

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
MsgStream& LoKi::Print::printHepMCDecay 
( const HepMC::GenParticle*    particle , 
  MsgStream&                   stream   , 
  const LoKi::GenTypes::GCuts& cut      ,
  const int                    level    , 
  const std::string&           blank    ) 
{
  if ( stream.isActive() ) 
  { LoKi::Print::printHepMCDecay 
      ( particle , stream.stream() , cut , level , blank ) ; }
  return stream ;
} ;
// ============================================================================
/// Simple function to print decay in more or less "readable" format 
// ============================================================================
std::string LoKi::Print::printHepMCDecay 
( const HepMC::GenParticle*    particle , 
  const LoKi::GenTypes::GCuts& cut      ,
  const int                    level    , 
  const std::string&           blank    ) 
{
  std::ostringstream stream ;
  LoKi::Print::printHepMCDecay ( particle , stream, cut , level , blank ) ;
  return stream.str() ;
} ;
// ============================================================================
/// Simple function to print decay in more or less "readable" format 
// ============================================================================
std::ostream& LoKi::Print::printHepMCDecay 
( const HepMC::GenParticle*    particle , 
  std::ostream&                stream   , 
  const LoKi::GenTypes::GCuts& cut      ,
  const int                    level    , 
  const std::string&           blank    ) 
{
  if ( 0 == particle    ) 
  { 
    LoKi::Report::Warning ( "LoKi::printDecay, invalid particle" ) ; 
    return stream << " <NULL> " ;                                 // RETURN 
  }
  //
  if ( !cut( particle ) ) { return stream << blank ; } ;          // RETURN 
  // own name 
  const std::string name = 
    LoKi::Particles::nameFromPID ( LHCb::ParticleID ( particle->pdg_id() ) ) ;
  HepMC::GenVertex* ev = particle->end_vertex() ;
  if ( 0 == ev      ) { return stream << " " << name << " " ; }   // RETURN
  HepMC::GenVertex::particle_iterator begin = 
    ev->particles_begin ( HepMC::children ) ;
  HepMC::GenVertex::particle_iterator end   = 
    ev->particles_end   ( HepMC::children ) ;
  if ( end == begin ) { return stream << " " << name << " " ; }  // RETURN
  //
  if ( 0 >= level   ) { return stream ; }                        // RETURN 
  // print the decay 
  stream << " ( " << name << " ->  " ;
  for ( ; begin != end ; ++begin ) 
  { LoKi::Print::printHepMCDecay 
      ( *begin , stream , cut , level - 1 , blank ) ; }          // RECURSION
  //
  return stream << " ) " ;                                       // RETURN  
} ;

// ============================================================================
// The END 
// ============================================================================

