// $Id: PrintHepMCDecay.cpp,v 1.8 2009-05-21 15:49:59 ibelyaev Exp $
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
// HepMC
// ============================================================================
#ifdef __INTEL_COMPILER
  #pragma warning (disable:1572) // floating-point equality and inequality comparisons are unreliable
  #pragma warning(push)
#endif
#include "HepMC/GenVertex.h"
#ifdef __INTEL_COMPILER
  #pragma warning(pop)
#endif
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/ParticleProperties.h"
#include "LoKi/Report.h"
#include "LoKi/Primitives.h"
// ===========================================================================
// LoKiGen
// ===========================================================================
#include "LoKi/GenTypes.h"
// ===========================================================================
// Local 
// ===========================================================================
#include "LoKi/PrintHepMCDecay.h"
#include "LoKi/GenOscillated.h"
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
 *  with the campain of \<XXXXXXX\> : 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-05-26 
 */
// ============================================================================
// Simple function to print decay in more or less "readable" format 
// ============================================================================
MsgStream& LoKi::PrintHepMC::printDecay 
( const HepMC::GenParticle*    particle , 
  MsgStream&                   stream   , 
  const LoKi::GenTypes::GCuts& cut      ,
  const int                    level    , 
  const std::string&           blank    ) 
{
  if ( stream.isActive() ) 
  { LoKi::PrintHepMC::printDecay 
      ( particle , stream.stream() , cut , level , blank ) ; }
  return stream ;
} 
// ============================================================================
// Simple function to print decay in more or less "readable" format 
// ============================================================================
std::string LoKi::PrintHepMC::printDecay 
( const HepMC::GenParticle*    particle , 
  const LoKi::GenTypes::GCuts& cut      ,
  const int                    level    , 
  const std::string&           blank    ) 
{
  std::ostringstream stream ;
  LoKi::PrintHepMC::printDecay ( particle , stream, cut , level , blank ) ;
  return stream.str() ;
} 
// ============================================================================
// Simple function to print decay in more or less "readable" format 
// ============================================================================
std::ostream& LoKi::PrintHepMC::printDecay 
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
  //
  const HepMC::GenParticle* osc1 = LoKi::GenParticles::oscillated1 ( particle ) ;
  const HepMC::GenParticle* osc2 = LoKi::GenParticles::oscillated2 ( particle ) ;  
  // 
  // case 1 :
  //      osc1 == osc2 == 0 
  //      no oscilation 
  // case 2 : 
  //      osc1 != 0 , osc2 == 0 
  //      oscillated "fictive" particle, print mother ( 'osc1' ) ID 
  //                               and own daughetrs  
  // case 3 : 
  //      osc1 == 0 , osc2 != 0 
  //      oscillated "real" particle, print own ID and grand-daughetrs  
  // case 4 : 
  //      osc1 != 0 , osc2 != 0 
  //      invalid case 
  if ( 0 != osc1 && 0 != osc2 ) 
  {
    LoKi::Report::Error ( "LoKi::printDecay, invalid oscillation pattern" ) ; 
    return stream << " <INVALID OSCILLATION PATTERN> " ;               // RETURN
  }
  const HepMC::GenParticle* self   = particle ;
  const HepMC::GenParticle* source = particle ;
  std::string name ;
  if ( 0 != osc1 ) 
  {
    self    = osc1     ;
    source  = particle ;
    name    = "[" + 
      LoKi::Particles::nameFromPID ( LHCb::ParticleID ( self->pdg_id() ) )
      + "]os''" ;
  }
  else if ( 0 != osc2 ) 
  {
    self   = particle ;
    source = osc2     ;
    name    = "[" + 
      LoKi::Particles::nameFromPID ( LHCb::ParticleID ( self->pdg_id() ) )
      + "]os'" ;
  }
  else 
  {
    self   = particle ;
    source = particle ;
    name   = LoKi::Particles::nameFromPID ( LHCb::ParticleID ( self->pdg_id() ) ) ;
  }

  // get daughters
  HepMC::GenVertex* ev = source->end_vertex() ;
  if ( 0 == ev      ) { return stream << " " << name << " " ; }   // RETURN
  HepMC::GenVertex::particle_iterator begin = 
    ev->particles_begin ( HepMC::children ) ;
  HepMC::GenVertex::particle_iterator end   = 
    ev->particles_end   ( HepMC::children ) ;
  if ( end == begin ) { return stream << " " << name << " " ; }  // RETURN
  // too many levels ?
  if ( 0 >= level   ) { return stream << " " << name << " " ; }  // RETURN
  // print the decay 
  stream << " ( " << name << " ->  " ;
  for ( ; begin != end ; ++begin ) 
  { LoKi::PrintHepMC::printDecay 
      ( *begin , stream , cut , level - 1 , blank ) ; }          // RECURSION
  //
  return stream << " ) " ;                                       // RETURN  
}
// ============================================================================
//  Simple function to print HepMC decay in more or less "readable" format 
// ============================================================================
std::ostream& LoKi::PrintHepMC::printDecay 
( const HepMC::GenParticle*      particle , 
  std::ostream&                  stream   ) 
{
  return LoKi::PrintHepMC::printDecay 
    ( particle , stream , 
      LoKi::BasicFunctors<const HepMC::GenParticle*>::BooleanConstant ( true ) ) ;  
}
// ============================================================================
//  Simple function to print HepMC decay in more or less "readable" format 
// ============================================================================ 
MsgStream& LoKi::PrintHepMC::printDecay 
( const HepMC::GenParticle*    particle , 
  MsgStream&                   stream   ) 
{
  return LoKi::PrintHepMC::printDecay 
    ( particle , stream , 
      LoKi::BasicFunctors<const HepMC::GenParticle*>::BooleanConstant ( true ) ) ;  
}
// ============================================================================
//  Simple function to print HepMC decay in more or less "readable" format 
// ============================================================================
std::string LoKi::PrintHepMC::printDecay 
( const HepMC::GenParticle*    particle ) 
{
  return LoKi::PrintHepMC::printDecay 
  ( particle , 
    LoKi::BasicFunctors<const HepMC::GenParticle*>::BooleanConstant ( true ) ) ;  
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

