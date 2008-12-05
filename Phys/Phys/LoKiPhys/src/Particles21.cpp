// $Id: Particles21.cpp,v 1.2 2008-12-05 09:09:21 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <cmath>
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/ParticleProperty.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/Particles21.h"
// ============================================================================
/** @file 
 *  implementation fiel for the functions form the file LoKi/Particles21.h
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-09-23
 */
// ============================================================================
// evaluate the delta mass 
// ============================================================================
double LoKi::Particles::DeltaNominalMass::delta
( LoKi::Particles::DeltaNominalMass::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ( "Invalid argument, return 'Invalid Mass'" ) ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  const LHCb::ParticleProperty* pp =
    LoKi::Particles::_ppFromPID ( p -> particleID() ) ;
  //
  if ( 0 == pp ) 
  {
    Error ( "Invalid particleID , return 'Invalid Mass'" ) ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  return p-> momentum(). M ()  - pp->mass () ;  
}
// ============================================================================
// evaluate the delta mass 
// ============================================================================
double LoKi::Particles::AbsDeltaNominalMass::adelta
( LoKi::Particles::AbsDeltaNominalMass::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ( "Invalid argument, return 'Invalid Mass'" ) ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  const LHCb::ParticleProperty* pp =
    LoKi::Particles::_ppFromPID ( p -> particleID() ) ;
  //
  if ( 0 == pp ) 
  {
    Error ( "Invalid particleID , return 'Invalid Mass'" ) ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  return ::fabs ( p->momentum() . M ()  - pp->mass () ) ;  
}
// ============================================================================
// evaluate the chi2 mass 
// ============================================================================
double LoKi::Particles::DeltaNominalMassChi2::chi2
( LoKi::Particles::DeltaNominalMassChi2::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ( "Invalid argument, return 'Invalid Chi2'" ) ;
    return LoKi::Constants::InvalidChi2 ;
  }
  //
  const LHCb::ParticleProperty* pp =
    LoKi::Particles::_ppFromPID ( p -> particleID() ) ;
  //
  if ( 0 == pp ) 
  {
    Error ( "Invalid particleID , return 'Invalid Chi2'" ) ;
    return LoKi::Constants::InvalidChi2 ;
  }
  //
  return LoKi::Kinematics::chi2mass 
    ( pp -> mass () , p -> momentum () , p -> momCovMatrix () ) ; 
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
