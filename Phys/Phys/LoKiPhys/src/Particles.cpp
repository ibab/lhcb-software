// $Id$
// ============================================================================
// Include files 
// ============================================================================
// Event
// ============================================================================
#include "Event/RecVertex.h"
#include "Event/ProtoParticle.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleFilter.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Particles.h"
#include "LoKi/ParticleContextCuts.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/format.hpp"
// ============================================================================
/** @file
 *
 *  Implementation file for functions from namespace  LoKi::Particles
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
 *  @date 2006-02-10 
 */
// ============================================================================
// get unique string for LHCb::Particle 
// ============================================================================
std::string LoKi::Particles::hex_id ( const LHCb::Particle* particle ) 
{
  if ( 0 == particle ) { return "NULL" ; }
  boost::format fmt ( "%p" ) ;
  const void * p = particle ;
  fmt % p ;
  return fmt.str() ;
}
// ============================================================================
// The END 
// ============================================================================

