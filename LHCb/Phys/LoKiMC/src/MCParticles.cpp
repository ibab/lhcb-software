// $Id$
// ============================================================================
// Include files
// ============================================================================
// STD &STL
// ============================================================================
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MCTypes.h"
#include "LoKi/MCParticles.h"
#include "LoKi/Kinematics.h"
#include "LoKi/MCTrees.h"
#include "LoKi/Print.h"
#include "LoKi/MCChild.h"
#include "LoKi/MCAlgs.h"
#include "LoKi/Math.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/ToCpp.h"
// ============================================================================
// MCInterfaces
// ============================================================================
#include "MCInterfaces/IMCDecayFinder.h"
#include "MCInterfaces/IMCParticleSelector.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/format.hpp"
// ============================================================================
/*  @file
 *
 *  Implementation file for functions from namespace LoKi::MCParticles 
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
LoKi::MCParticles::Momentum::result_type 
LoKi::MCParticles::Momentum::operator() 
  ( LoKi::MCParticles::Momentum::argument p ) const
{
  if ( 0 != p ) { return p -> momentum ().P() ; }    // RETURN 
  Error (" Invalid Particle, return 'InvalidMomenum'") ;
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
LoKi::MCParticles::Momentum*
LoKi::MCParticles::Momentum::clone() const 
{ return new LoKi::MCParticles::Momentum(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::Momentum::fillStream( std::ostream& s ) const 
{ return s << "MCP" ; }
// ============================================================================
LoKi::MCParticles::Energy::result_type 
LoKi::MCParticles::Energy::operator() 
  ( LoKi::MCParticles::Energy::argument p ) const
{
  if ( 0 != p ) { return p -> momentum () .e ()  ; }          // RETURN 
  Error (" Invalid Particle, return 'InvalidEnergy'") ;
  return LoKi::Constants::InvalidEnergy;                     // RETURN 
}
// ============================================================================
LoKi::MCParticles::Energy*
LoKi::MCParticles::Energy::clone() const 
{ return new LoKi::MCParticles::Energy(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::Energy::fillStream( std::ostream& s ) const 
{ return s << "MCE" ; }
// ============================================================================
LoKi::MCParticles::Oscillated::result_type 
LoKi::MCParticles::Oscillated::operator() 
  ( LoKi::MCParticles::Oscillated::argument p ) const
{
  if ( 0 != p ) { return p -> hasOscillated() ; }          // RETURN 
  Error (" Invalid Particle, return 'false'") ;
  return false ;                     // RETURN 
}
// ============================================================================
LoKi::MCParticles::Oscillated*
LoKi::MCParticles::Oscillated::clone() const 
{ return new LoKi::MCParticles::Oscillated(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::Oscillated::fillStream( std::ostream& s ) const 
{ return s << "MCOSCILLATED" ; }
// ============================================================================
LoKi::MCParticles::TransverseMomentum::result_type 
LoKi::MCParticles::TransverseMomentum::operator() 
  ( LoKi::MCParticles::TransverseMomentum::argument p ) const
{
  if ( 0 != p ) { return p -> momentum().Pt() ; }           // RETURN 
  Error(" Invalid Particle, return 'InvalidMomentum'") ;
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
LoKi::MCParticles::TransverseMomentum*
LoKi::MCParticles::TransverseMomentum::clone() const 
{ return new LoKi::MCParticles::TransverseMomentum(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::TransverseMomentum::fillStream
( std::ostream& s ) const 
{ return s << "MCPT" ; }
// ============================================================================
LoKi::MCParticles::MomentumX::result_type 
LoKi::MCParticles::MomentumX::operator() 
  ( LoKi::MCParticles::MomentumX::argument p ) const
{
  if( 0 != p ) { return p -> momentum () . px () ; }    // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'") ;
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
LoKi::MCParticles::MomentumX*
LoKi::MCParticles::MomentumX::clone() const 
{ return new LoKi::MCParticles::MomentumX(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::MomentumX::fillStream
( std::ostream& s ) const 
{ return s << "MCPX" ; }
// ============================================================================
LoKi::MCParticles::MomentumY::result_type 
LoKi::MCParticles::MomentumY::operator() 
  ( LoKi::MCParticles::MomentumY::argument p ) const
{
  if( 0 != p ) { return p -> momentum () . py () ; }    // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'") ;
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
LoKi::MCParticles::MomentumY*
LoKi::MCParticles::MomentumY::clone() const 
{ return new LoKi::MCParticles::MomentumY(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::MomentumY::fillStream
( std::ostream& s ) const 
{ return s << "MCPY" ; }
// ============================================================================
LoKi::MCParticles::MomentumZ::result_type 
LoKi::MCParticles::MomentumZ::operator() 
  ( LoKi::MCParticles::MomentumZ::argument p ) const
{
  if( 0 != p ) { return p -> momentum () . pz () ; }    // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'") ;
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
LoKi::MCParticles::MomentumZ*
LoKi::MCParticles::MomentumZ::clone() const 
{ return new LoKi::MCParticles::MomentumZ(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::MomentumZ::fillStream
( std::ostream& s ) const 
{ return s << "MCPZ" ; }
// ============================================================================
LoKi::MCParticles::PseudoRapidity::result_type 
LoKi::MCParticles::PseudoRapidity::operator() 
  ( LoKi::MCParticles::PseudoRapidity::argument p ) const
{
  if( 0 != p ) { return eta ( p ) ; }    // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'") ;
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
LoKi::MCParticles::PseudoRapidity*
LoKi::MCParticles::PseudoRapidity::clone() const 
{ return new LoKi::MCParticles::PseudoRapidity(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::PseudoRapidity::fillStream
( std::ostream& s ) const 
{ return s << "MCETA" ; }
// ============================================================================
double LoKi::MCParticles::PseudoRapidity::y 
( const Gaudi::LorentzVector& v ) const
{
  const double e  = v.E  () ;
  const double pz = v.Pz () ;
  return 0.5*std::log( (e+pz)/(e-pz) ) ;
}
// ============================================================================
double LoKi::MCParticles::PseudoRapidity::y0
( const Gaudi::LorentzVector& v ) const
{
  const double e  = v.E  () ;
  const double p  = v.P  () ;
  return 0.5*std::log( (e+p)/(e-p) ) ;
}
// ============================================================================
LoKi::MCParticles::Rapidity*
LoKi::MCParticles::Rapidity::clone() const 
{ return new LoKi::MCParticles::Rapidity(*this) ; }
// ============================================================================
LoKi::MCParticles::Rapidity0*
LoKi::MCParticles::Rapidity0::clone() const 
{ return new LoKi::MCParticles::Rapidity0(*this) ; }
// ============================================================================
LoKi::MCParticles::Rapidity::result_type 
LoKi::MCParticles::Rapidity::operator() 
  ( LoKi::MCParticles::Rapidity::argument p ) const
{
  if ( 0 != p ) { return y ( p ) ; }    // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'") ;
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
LoKi::MCParticles::Rapidity0::result_type 
LoKi::MCParticles::Rapidity0::operator() 
  ( LoKi::MCParticles::Rapidity0::argument p ) const
{
  if ( 0 != p ) { return y0 ( p ) ; }    // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'") ;
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
std::ostream&
LoKi::MCParticles::Rapidity::fillStream
( std::ostream& s ) const 
{ return s << "MCY" ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::Rapidity0::fillStream
( std::ostream& s ) const 
{ return s << "MCY0" ; }
// ============================================================================


// ============================================================================
LoKi::MCParticles::Theta::result_type 
LoKi::MCParticles::Theta::operator() 
  ( LoKi::MCParticles::Theta::argument p ) const
{
  if( 0 != p ) { return p -> momentum () . theta () ; }    // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
LoKi::MCParticles::Theta*
LoKi::MCParticles::Theta::clone() const 
{ return new LoKi::MCParticles::Theta(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::Theta::fillStream
( std::ostream& s ) const 
{ return s << "MCTHETA" ; }
// ============================================================================
LoKi::MCParticles::Phi::result_type 
LoKi::MCParticles::Phi::operator() 
  ( LoKi::MCParticles::Phi::argument p ) const
{
  if ( 0 != p ) { return adjust ( phi ( p ) ) ; }    // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
LoKi::MCParticles::Phi*
LoKi::MCParticles::Phi::clone() const 
{ return new LoKi::MCParticles::Phi(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::Phi::fillStream
( std::ostream& s ) const 
{ return s << "MCPHI" ; }
// ============================================================================
LoKi::MCParticles::Mass::result_type 
LoKi::MCParticles::Mass::operator() 
  ( LoKi::MCParticles::Mass::argument p ) const
{
  if( 0 != p ) { return p -> momentum () . M  () ; }     // RETURN 
  Error(" Invalid Particle, return 'InvalidMass'");
  return LoKi::Constants::InvalidMass;                   // RETURN 
}
// ============================================================================
LoKi::MCParticles::Mass*
LoKi::MCParticles::Mass::clone() const 
{ return new LoKi::MCParticles::Mass(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::Mass::fillStream
( std::ostream& s ) const 
{ return s << "MCMASS" ; }
// ============================================================================
LoKi::MCParticles::Identifier::result_type 
LoKi::MCParticles::Identifier::operator() 
  ( LoKi::MCParticles::Identifier::argument p ) const  
{
  if( 0 != p ) { return p -> particleID() . pid () ; }         // RETURN 
  Error(" Invalid Particle, return 'InvalidID'");
  return LoKi::Constants::InvalidID;                           // RETURN 
}
// ============================================================================
LoKi::MCParticles::Identifier*
LoKi::MCParticles::Identifier::clone() const 
{ return new LoKi::MCParticles::Identifier(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::Identifier::fillStream
( std::ostream& s ) const 
{ return s << "MCID" ; }
// ============================================================================
LoKi::MCParticles::AbsIdentifier::result_type 
LoKi::MCParticles::AbsIdentifier::operator() 
  ( LoKi::MCParticles::AbsIdentifier::argument p ) const  
{
  if( 0 != p ) { return p -> particleID() . abspid () ; }     // RETURN 
  Error(" Invalid Particle, return 'InvalidID'");
  return LoKi::Constants::InvalidID;                           // RETURN 
}
// ============================================================================
// IsID 
// ============================================================================
LoKi::MCParticles::IsID::IsID ( const long                      id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate() 
  , LoKi::Pids::GetPids ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsID::IsID( const unsigned long  id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate() 
  , LoKi::Pids::GetPids ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsID::IsID( const LHCb::ParticleID&         id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate() 
  , LoKi::Pids::GetPids ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsID::IsID( const std::string& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate() 
  , LoKi::Pids::GetPids ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsID::IsID( const std::vector<int>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate() 
  , LoKi::Pids::GetPids ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsID::IsID( const std::vector<unsigned int>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate() 
  , LoKi::Pids::GetPids ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsID::IsID( const std::vector<long>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate() 
  , LoKi::Pids::GetPids ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsID::IsID( const std::vector<unsigned long>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate() 
  , LoKi::Pids::GetPids ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsID::IsID( const std::vector<LHCb::ParticleID>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate() 
  , LoKi::Pids::GetPids ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsID::IsID( const std::vector<std::string>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate() 
  , LoKi::Pids::GetPids ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsID::IsID( const LoKi::Pids::GetPids& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate() 
  , LoKi::Pids::GetPids ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsID*
LoKi::MCParticles::IsID::clone() const 
{ return new LoKi::MCParticles::IsID(*this) ; }
// ============================================================================
std::ostream& LoKi::MCParticles::IsID::fillStream( std::ostream& s ) const
{
  //
  s << "(MCID== " ;
  //
  if      ( 1 == m_names.size() ) { Gaudi::Utils::toStream ( m_names[0] , s ) ; }
  else if ( !m_names.empty()    ) { Gaudi::Utils::toStream ( m_names    , s ) ; }
  else if ( 1 == m_ints.size()  ) { Gaudi::Utils::toStream ( m_ints [0] , s ) ; }
  else                            { Gaudi::Utils::toStream ( m_ints     , s ) ; }
  //
  return s << ")" ; 
}
// ============================================================================
// the only one essential method
// ============================================================================
LoKi::MCParticles::IsID::result_type 
LoKi::MCParticles::IsID::operator() 
  ( LoKi::MCParticles::IsID::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error("Invalid Particle, return 'False'");
    return false ;                     // RETURN 
  }
  //
  return in_list ( p->particleID () ) ;  
}
// ============================================================================
// IsNotID 
// ============================================================================
LoKi::MCParticles::IsNotID::IsNotID ( const long                id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotID::IsNotID( const unsigned long  id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotID::IsNotID( const LHCb::ParticleID&         id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotID::IsNotID( const std::string& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotID::IsNotID( const std::vector<int>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotID::IsNotID( const std::vector<unsigned int>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotID::IsNotID( const std::vector<long>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotID::IsNotID( const std::vector<unsigned long>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotID::IsNotID( const std::vector<LHCb::ParticleID>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotID::IsNotID( const std::vector<std::string>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotID*
LoKi::MCParticles::IsNotID::clone() const 
{ return new LoKi::MCParticles::IsNotID(*this) ; }
// ============================================================================
std::ostream& LoKi::MCParticles::IsNotID::fillStream( std::ostream& s ) const
{
  //
  s << "(MCID!= " ;
  //
  if      ( 1 == m_names.size() ) { Gaudi::Utils::toStream ( m_names[0] , s ) ; }
  else if ( !m_names.empty()    ) { Gaudi::Utils::toStream ( m_names    , s ) ; }
  else if ( 1 == m_ints.size()  ) { Gaudi::Utils::toStream ( m_ints [0] , s ) ; }
  else                            { Gaudi::Utils::toStream ( m_ints     , s ) ; }
  //
  return s << ")" ; 
}
// ============================================================================
// the only one essential method
// ============================================================================
LoKi::MCParticles::IsNotID::result_type 
LoKi::MCParticles::IsNotID::operator() 
  ( LoKi::MCParticles::IsNotID::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error("Invalid Particle, return 'False'");
    return false ;                     // RETURN 
  }
  return not_in_list ( p->particleID() ) ;
}


// ============================================================================
// IsAbsID 
// ============================================================================
LoKi::MCParticles::IsAbsID::IsAbsID ( const long id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsAbsID::IsAbsID ( const unsigned long  id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsAbsID::IsAbsID ( const LHCb::ParticleID&         id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsAbsID::IsAbsID ( const std::string& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsAbsID::IsAbsID ( const std::vector<int>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsAbsID::IsAbsID ( const std::vector<unsigned int>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsAbsID::IsAbsID ( const std::vector<long>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsAbsID::IsAbsID ( const std::vector<unsigned long>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsAbsID::IsAbsID ( const std::vector<LHCb::ParticleID>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsAbsID::IsAbsID ( const std::vector<std::string>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsAbsID::IsAbsID ( const LoKi::Pids::GetPids& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsAbsID*
LoKi::MCParticles::IsAbsID::clone() const 
{ return new LoKi::MCParticles::IsAbsID(*this) ; }
// ============================================================================
std::ostream& LoKi::MCParticles::IsAbsID::fillStream( std::ostream& s ) const
{
  //
  s << "(MCABSID== " ;
  //
  if      ( 1 == m_names.size() ) { Gaudi::Utils::toStream ( m_names[0] , s ) ; }
  else if ( !m_names.empty()    ) { Gaudi::Utils::toStream ( m_names    , s ) ; }
  else if ( 1 == m_ints.size()  ) { Gaudi::Utils::toStream ( m_ints [0] , s ) ; }
  else                            { Gaudi::Utils::toStream ( m_ints     , s ) ; }
  //
  return s << ")" ; 
}
// ============================================================================
// the only one essential method
// ============================================================================
LoKi::MCParticles::IsAbsID::result_type 
LoKi::MCParticles::IsAbsID::operator() 
  ( LoKi::MCParticles::IsAbsID::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error("Invalid Particle, return 'False'");
    return false ;                     // RETURN 
  }
  //
  return in_abs_list ( p->particleID() ) ;
}




// ============================================================================
// IsNotAbsID 
// ============================================================================
LoKi::MCParticles::IsNotAbsID::IsNotAbsID ( const long         id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsAbsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotAbsID::IsNotAbsID ( const unsigned long id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsAbsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotAbsID::IsNotAbsID ( const LHCb::ParticleID&         id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsAbsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotAbsID::IsNotAbsID ( const std::string& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsAbsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotAbsID::IsNotAbsID ( const std::vector<int>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsAbsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotAbsID::IsNotAbsID ( const std::vector<unsigned int>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsAbsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotAbsID::IsNotAbsID ( const std::vector<long>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsAbsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotAbsID::IsNotAbsID ( const std::vector<unsigned long>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsAbsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotAbsID::IsNotAbsID ( const std::vector<LHCb::ParticleID>& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsAbsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotAbsID::IsNotAbsID ( const std::vector<std::string>& id  )  
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsAbsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotAbsID::IsNotAbsID ( const LoKi::Pids::GetPids& id  ) 
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , LoKi::MCParticles::IsAbsID ( id ) 
{}
// ============================================================================
LoKi::MCParticles::IsNotAbsID*
LoKi::MCParticles::IsNotAbsID::clone() const 
{ return new LoKi::MCParticles::IsNotAbsID(*this) ; }
// ============================================================================
std::ostream& LoKi::MCParticles::IsNotAbsID::fillStream( std::ostream& s ) const
{
  //
  s << "(MCABSID!= " ;
  //
  if      ( 1 == m_names.size() ) { Gaudi::Utils::toStream ( m_names[0] , s ) ; }
  else if ( !m_names.empty()    ) { Gaudi::Utils::toStream ( m_names    , s ) ; }
  else if ( 1 == m_ints.size()  ) { Gaudi::Utils::toStream ( m_ints [0] , s ) ; }
  else                            { Gaudi::Utils::toStream ( m_ints     , s ) ; }
  //
  return s << ")" ; 
}
// ============================================================================
// the only one essential method
// ============================================================================
LoKi::MCParticles::IsNotAbsID::result_type 
LoKi::MCParticles::IsNotAbsID::operator() 
  ( LoKi::MCParticles::IsNotAbsID::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error("Invalid Particle, return 'False'");
    return false ;                     // RETURN 
  }
  //
  return not_in_abs_list ( p->particleID() ) ;
}
// ============================================================================





















// ============================================================================
LoKi::MCParticles::AbsIdentifier*
LoKi::MCParticles::AbsIdentifier::clone() const 
{ return new LoKi::MCParticles::AbsIdentifier(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::AbsIdentifier::fillStream
( std::ostream& s ) const 
{ return s << "MCABSID" ; }
// ============================================================================
LoKi::MCParticles::ThreeCharge::result_type 
LoKi::MCParticles::ThreeCharge::operator() 
  ( LoKi::MCParticles::ThreeCharge::argument p ) const  
{
  if( 0 != p ) { return p -> particleID().threeCharge()  ; } // RETURN 
  Error(" Invalid Particle, return 'InvalidCharge'");
  return LoKi::Constants::InvalidCharge;                     // RETURN 
}
// ============================================================================
LoKi::MCParticles::ThreeCharge*
LoKi::MCParticles::ThreeCharge::clone() const 
{ return new LoKi::MCParticles::ThreeCharge(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::ThreeCharge::fillStream
( std::ostream& s ) const 
{ return s << "MC3Q" ; }
// ============================================================================
LoKi::MCParticles::ProperLifeTime::result_type 
LoKi::MCParticles::ProperLifeTime::operator() 
  ( LoKi::MCParticles::ProperLifeTime::argument p ) const
{
  if ( 0 == p ) 
  {
    Error(" Invalid Particle, return 'InvalidTime'");
    return LoKi::Constants::InvalidTime;                     // RETURN 
  }
  // 
  if ( p->endVertices().empty() ) 
  { return LoKi::Constants::InfiniteTime ; }                // RETURN}
  //
  const LHCb::MCVertex* endVertex = p->endVertices()[0] ;
  if ( 0 == endVertex ) 
  {
    Error (" Invalid 'endVertex' , return 'InfiniteTime'");
    return LoKi::Constants::InfiniteTime ;                  // RETURN 
  };
  const LHCb::MCVertex* originVertex = p->originVertex() ;
  if ( 0 == originVertex ) 
  {
    Error(" Invalid 'originVertex' , return 'InvalidTime'");
    return LoKi::Constants::InvalidTime;                     // RETURN 
  }
  // evaluate the distance 
  const double dist = ( endVertex     -> position() - 
                        originVertex  -> position() ).R();
  // recalculate distance to the time 
  const double mass     = p->momentum().M() ;
  const double mom      = p->momentum().P() ;
  
  if ( 0 >= mass ) 
  { 
    Error (" Mass is invalid, return 'InvalidTime' ");  
    return  LoKi::Constants::InvalidTime ;              // RETURN 
  }  
  if ( 0 >= mom  ) 
  { 
    Error (" |Momentum| is invalid, return 'InvalidTime' ");
    return  LoKi::Constants::InvalidTime ;              // RETURN 
  }  
  const double betagamma   = mom / mass ;
  //
  if ( 0 >= betagamma  ) 
  { 
    Error (" betagamma is invalid, return 'InvalidTime' ");
    return  LoKi::Constants::InvalidTime ;              // RETURN 
  }  
  //
  return dist/betagamma ;
}
// ============================================================================
LoKi::MCParticles::ProperLifeTime*
LoKi::MCParticles::ProperLifeTime::clone() const 
{ return new LoKi::MCParticles::ProperLifeTime(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::ProperLifeTime::fillStream
( std::ostream& s ) const 
{ return s << "MCCTAU" ; }
// ============================================================================
//  constructor from Quark 
// ============================================================================
LoKi::MCParticles::HasQuark::HasQuark ( const LHCb::ParticleID::Quark quark ) 
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate() 
  , m_quark ( quark  )
{}
// ============================================================================
LoKi::MCParticles::HasQuark::HasQuark 
( const LoKi::MCParticles::HasQuark& right ) 
  : LoKi::AuxFunBase                        ( right ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate( right ) 
  , m_quark ( right.m_quark  )
{}
// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::MCParticles::HasQuark::result_type 
LoKi::MCParticles::HasQuark::operator() 
  ( LoKi::MCParticles::HasQuark::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("MCParticle* point to NULL, returs 'false'");
    return false ;
  }
  return p->particleID().hasQuark( quark() ) ;
}
// ============================================================================
LoKi::MCParticles::HasQuark*
LoKi::MCParticles::HasQuark::clone() const 
{ return new LoKi::MCParticles::HasQuark(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::HasQuark::fillStream
( std::ostream& s ) const 
{
  switch ( quark() ) 
  {
  case LHCb::ParticleID::top      :
    return s << "TOP"     ;
    break                 ;
  case LHCb::ParticleID::bottom   :
    return s << "BEAUTY"  ;
    break                 ;
  case LHCb::ParticleID::charm    :
    return s << "CHARM"   ;
    break                 ;
  case LHCb::ParticleID::strange  :
    return s << "STRANGE" ;
    break                 ;
  default                 :
    return s << "MCQUARK(" << quark() << ")" ;
    break                 ;
  }
}
// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::MCParticles::IsCharged::result_type 
LoKi::MCParticles::IsCharged::operator() 
  ( LoKi::MCParticles::IsCharged::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("MCParticle* point to NULL, returs 'false'");
    return false ;
  }
  return 0 != p->particleID().threeCharge() ;
}
// ============================================================================
LoKi::MCParticles::IsCharged*
LoKi::MCParticles::IsCharged::clone() const 
{ return new LoKi::MCParticles::IsCharged(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::IsCharged::fillStream
( std::ostream& s ) const 
{ return s << "CHARGED" ; }
// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::MCParticles::IsNeutral::result_type 
LoKi::MCParticles::IsNeutral::operator() 
  ( LoKi::MCParticles::IsNeutral::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("MCParticle* point to NULL, returs 'false'");
    return false ;
  }
  return 0 == p->particleID().threeCharge() ;
}
// ============================================================================
LoKi::MCParticles::IsNeutral*
LoKi::MCParticles::IsNeutral::clone() const 
{ return new LoKi::MCParticles::IsNeutral(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::IsNeutral::fillStream
( std::ostream& s ) const 
{ return s << "NEUTRAL" ; }
// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::MCParticles::IsLepton::result_type 
LoKi::MCParticles::IsLepton::operator() 
  ( LoKi::MCParticles::IsLepton::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("MCParticle* point to NULL, returs 'false'");
    return false ;
  }
  return p->particleID().isLepton() ;
}
// ============================================================================
LoKi::MCParticles::IsLepton*
LoKi::MCParticles::IsLepton::clone() const 
{ return new LoKi::MCParticles::IsLepton(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::IsLepton::fillStream
( std::ostream& s ) const 
{ return s << "LEPTON" ; }
// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::MCParticles::IsMeson::result_type 
LoKi::MCParticles::IsMeson::operator() 
  ( LoKi::MCParticles::IsMeson::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("MCParticle* point to NULL, returs 'false'");
    return false ;
  }
  return p->particleID().isMeson() ;
}
// ============================================================================
LoKi::MCParticles::IsMeson*
LoKi::MCParticles::IsMeson::clone() const 
{ return new LoKi::MCParticles::IsMeson(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::IsMeson::fillStream
( std::ostream& s ) const 
{ return s << "MESON" ; }
// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::MCParticles::IsBaryon::result_type 
LoKi::MCParticles::IsBaryon::operator() 
  ( LoKi::MCParticles::IsBaryon::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("MCParticle* point to NULL, returs 'false'");
    return false ;
  }
  return p->particleID().isBaryon() ;
}
// ============================================================================
LoKi::MCParticles::IsBaryon*
LoKi::MCParticles::IsBaryon::clone() const 
{ return new LoKi::MCParticles::IsBaryon(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::IsBaryon::fillStream
( std::ostream& s ) const 
{ return s << "BARYON" ; }
// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::MCParticles::IsHadron::result_type 
LoKi::MCParticles::IsHadron::operator() 
  ( LoKi::MCParticles::IsHadron::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("MCParticle* point to NULL, returs 'false'");
    return false ;
  }
  return p->particleID().isHadron() ;
}
// ============================================================================
LoKi::MCParticles::IsHadron*
LoKi::MCParticles::IsHadron::clone() const 
{ return new LoKi::MCParticles::IsHadron(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::IsHadron::fillStream
( std::ostream& s ) const 
{ return s << "HADRON" ; }
// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::MCParticles::IsNucleus::result_type 
LoKi::MCParticles::IsNucleus::operator() 
  ( LoKi::MCParticles::IsNucleus::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("MCParticle* point to NULL, returs 'false'");
    return false ;
  }
  return p->particleID().isNucleus() ;
}
// ============================================================================
LoKi::MCParticles::IsNucleus*
LoKi::MCParticles::IsNucleus::clone() const 
{ return new LoKi::MCParticles::IsNucleus(*this) ; }
// ============================================================================
std::ostream&
LoKi::MCParticles::IsNucleus::fillStream
( std::ostream& s ) const 
{ return s << "NUCLEUS" ; }
// ============================================================================
LoKi::MCParticles::FromMCDecayTree::FromMCDecayTree
( const LHCb::MCParticle* mcp )
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate()
  , m_cont ( 1 , mcp ) {}
// ============================================================================
LoKi::MCParticles::FromMCDecayTree::FromMCDecayTree
( const LoKi::MCTypes::MCRange& cnt  )
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate()
  , m_cont ( cnt.begin() , cnt.end() ) {}
// ============================================================================
LoKi::MCParticles::FromMCDecayTree::FromMCDecayTree
( const LoKi::MCTypes::MCContainer& cnt  )
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate()
  , m_cont ( cnt.begin() , cnt.end() ) {}
// ============================================================================
LoKi::MCParticles::FromMCDecayTree::FromMCDecayTree
( const LHCb::MCParticles* cnt  )
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate()
  , m_cont () 
{
  if ( 0 == cnt ) { Error(" ::MCParticles* points to NULL!");}
  else            
  { 
    m_cont.reserve ( cnt->size() ) ;
    std::copy( cnt->begin() , cnt->end() , std::back_inserter ( m_cont ) ) ; 
  }
}
// ============================================================================
LoKi::MCParticles::FromMCDecayTree::FromMCDecayTree
( const LoKi::MCParticles::FromMCDecayTree& right ) 
  : LoKi::AuxFunBase                        ( right )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate( right )
  , m_cont ( right.m_cont ) {}
// ============================================================================
LoKi::MCParticles::FromMCDecayTree::~FromMCDecayTree(){}
// ============================================================================
LoKi::MCParticles::FromMCDecayTree* 
LoKi::MCParticles::FromMCDecayTree::clone() const 
{ return new FromMCDecayTree(*this) ; }
// ============================================================================
LoKi::MCParticles::FromMCDecayTree::result_type 
LoKi::MCParticles::FromMCDecayTree::operator() 
  ( LoKi::MCParticles::FromMCDecayTree::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error( "MCParticle* points to NULL, return 'false'");
    return false ;
  }  
  for ( MCCont::const_iterator it = m_cont.begin() ; 
        m_cont.end() != it ; ++it ) 
  {
    const LHCb::MCParticle* parent = *it ;
    if ( 0 == parent ) 
    { Error("MCParticle* point to NULL, skip it ") ;    continue ; }
    if ( LoKi::MCTrees::fromMCTree( p , parent ) ) { return true ; } // RETURN
  }
  return false ;
}
// ============================================================================
/*  the add MCparticle into the list of tested particles 
 *  @param p pointet to the particle 
 *  @return status code 
 */
// ============================================================================
StatusCode  LoKi::MCParticles::FromMCDecayTree::add ( const LHCb::MCParticle* p ) 
{
  if ( 0 == p ) 
  { Error ("add(): MCParticle* points to NULL") ; return StatusCode::FAILURE ; }
  m_cont.push_back( p ) ;
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  remove MCParticle from the list of tested MCParticles 
 *  @param p pointe to the MCParticle 
 *  @return status code 
 */
// ============================================================================
StatusCode  LoKi::MCParticles::FromMCDecayTree::remove ( const LHCb::MCParticle* p )
{
  MCCont::iterator it = 
    std::remove ( m_cont.begin() , m_cont.end() , p ) ;
  m_cont.erase ( it , m_cont.end() ) ;
  return StatusCode::SUCCESS ;
}
// ============================================================================
std::ostream&
LoKi::MCParticles::FromMCDecayTree::fillStream
( std::ostream& s ) const 
{ return s << "FROMMCTREE" ; }
// ============================================================================
LoKi::MCParticles::NinMCdownTree::NinMCdownTree
( const LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate& cut ) 
  : LoKi::AuxFunBase ( std::tie ( cut ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function() 
  , m_cut ( cut )
{}
// ============================================================================
LoKi::MCParticles::NinMCdownTree::~NinMCdownTree(){}
// ============================================================================
LoKi::MCParticles::NinMCdownTree*
LoKi::MCParticles::NinMCdownTree::clone() const
{ return new LoKi::MCParticles::NinMCdownTree(*this); }
// ============================================================================
LoKi::MCParticles::NinMCdownTree::result_type
LoKi::MCParticles::NinMCdownTree::operator() 
  ( LoKi::MCParticles::NinMCdownTree::argument p ) const 
{
  if ( 0 == p ) { Warning ( "MCParticle* points to NULL" ) ; }
  
  double number = 0 ;
  // check the particle itself 
  if ( m_cut( p ) ) { number += 1 ; }
  
  // traverse the tree 
  
  // the traversal is not possible 
  if ( 0 == p     ) { return number ; }
  
  typedef SmartRefVector<LHCb::MCVertex>   EndVertices ;
  typedef SmartRefVector<LHCb::MCParticle> Products    ;
  
  const EndVertices& endVertices = p->endVertices() ;
  for ( EndVertices::const_iterator iv = endVertices.begin() ; 
        endVertices.end() != iv ; ++iv ) 
  {
    const LHCb::MCVertex* vertex = *iv ;
    if ( 0 == vertex ) { continue ; }
    const Products& products = vertex->products() ;
    for ( Products::const_iterator ip = products.begin() ; 
          products.end() != ip ; ++ip ) 
    {
      const LHCb::MCParticle* mcp = *ip ;
      if ( 0 == mcp ) { continue ; }
      // start the recursion 
      number += (*this)(mcp) ;
    };
  }
  return number ;
}
// ============================================================================
std::ostream&
LoKi::MCParticles::NinMCdownTree::fillStream
( std::ostream& s ) const 
{ return s << "NINMCDOWN(" << m_cut << ")" ; }
// ============================================================================
LoKi::MCParticles::MCMotherFunction::MCMotherFunction
( const LoKi::MCParticles::MCMotherFunction& right ) 
  : LoKi::AuxFunBase                        ( right )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function ( right ) 
  , m_val ( right.m_val ) 
  , m_fun ( right.m_fun ) 
{}
// ============================================================================
LoKi::MCParticles::MCMotherFunction::MCMotherFunction
( const LoKi::BasicFunctors<const LHCb::MCParticle*>::Function& fun , 
  const double                                                  val )
  : LoKi::AuxFunBase ( std::tie ( fun , val ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function() 
  , m_val ( val ) 
  , m_fun ( fun ) 
{}
// ============================================================================
LoKi::MCParticles::MCMotherFunction::~MCMotherFunction(){}
// ============================================================================
LoKi::MCParticles::MCMotherFunction*
LoKi::MCParticles::MCMotherFunction::clone() const 
{ return new LoKi::MCParticles::MCMotherFunction( *this ) ; }
// ============================================================================
LoKi::MCParticles::MCMotherFunction::result_type 
LoKi::MCParticles::MCMotherFunction::operator() 
  ( LoKi::MCParticles::MCMotherFunction::argument p ) const 
{
  if ( 0 == p ) 
  { Error ( ": MCParticle* point to NULL, return -1.e+6" ) ; return -1.e+6 ; }
  const LHCb::MCParticle* mother = p->mother() ;
  if ( 0 == mother ) { return m_val ; }
  return m_fun ( mother ) ;
}
// ============================================================================
std::ostream&
LoKi::MCParticles::MCMotherFunction::fillStream
( std::ostream& s ) const 
{ return s << "MCMOTHER(" << m_fun << "," << m_val<< ")" ; }
// ============================================================================
LoKi::MCParticles::MCMotherPredicate::MCMotherPredicate
( const LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate& cut , 
  const bool                                val ) 
  : LoKi::AuxFunBase ( std::tie ( cut , val ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate() 
  , m_val ( val ) 
  , m_cut ( cut ) 
{}
// ============================================================================
LoKi::MCParticles::MCMotherPredicate::MCMotherPredicate
( const LoKi::MCParticles::MCMotherPredicate& right ) 
  : LoKi::AuxFunBase                        ( right )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate( right ) 
  , m_val ( right.m_val ) 
  , m_cut ( right.m_cut ) 
{}
// ============================================================================
LoKi::MCParticles::MCMotherPredicate::~MCMotherPredicate(){}
// ============================================================================
LoKi::MCParticles::MCMotherPredicate*
LoKi::MCParticles::MCMotherPredicate::clone() const 
{ return new LoKi::MCParticles::MCMotherPredicate( *this ) ; }
// ============================================================================
LoKi::MCParticles::MCMotherPredicate::result_type 
LoKi::MCParticles::MCMotherPredicate::operator() 
  ( LoKi::MCParticles::MCMotherPredicate::argument p ) const 
{
  if ( 0 == p ) 
  { Error ( ": MCParticle* point to NULL, return 'false'" ) ; return false ; }
  const LHCb::MCParticle* mother = p->mother() ;
  if ( 0 == mother ) { return m_val ; }
  return m_cut ( mother ) ;
}
// ============================================================================
std::ostream&
LoKi::MCParticles::MCMotherPredicate::fillStream
( std::ostream& s ) const 
{ return s << "MCMOTHCUT(" << m_cut << ( m_val ? ",True" : ",False" ) << ")" ; }
// ============================================================================
/*  constructor from 1 particle 
 *  @param object object to be compared 
 */
// ============================================================================
LoKi::MCParticles::IsParticle::IsParticle
( const LHCb::MCParticle* object ) 
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate() 
  , m_objects ( 1 , object )
{}
// ============================================================================
/*  constructor from range of particles 
 *  @param range range of object to be compared 
 */
// ============================================================================
LoKi::MCParticles::IsParticle::IsParticle
( const LoKi::MCTypes::MCRange& range ) 
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate() 
  , m_objects ( range.begin() , range.end() )
{}
// ============================================================================
/*  copy constructor
 *  @param right  object to be copied 
 */
// ============================================================================
LoKi::MCParticles::IsParticle::IsParticle
( const LoKi::MCParticles::IsParticle& right ) 
  : LoKi::AuxFunBase                        ( right )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate( right ) 
  , m_objects ( right.m_objects )
{}
// ============================================================================
//  destructor (virtual)
// ============================================================================
LoKi::MCParticles::IsParticle::~IsParticle() { m_objects.clear() ; }
// ============================================================================
//  MANDATORY: clone method ('virtual constructor')
// ============================================================================
LoKi::MCParticles::IsParticle*
LoKi::MCParticles::IsParticle::clone() const 
{ return new LoKi::MCParticles::IsParticle( *this ) ; }
// ============================================================================
//  MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::IsParticle::result_type
LoKi::MCParticles::IsParticle::operator() 
  ( LoKi::MCParticles::IsParticle::argument p ) const 
{
  LHCb::MCParticle::ConstVector::const_iterator it = 
    std::find( m_objects.begin() , m_objects.end() , p ) ;
  return m_objects.end() != it ;
}
// ============================================================================
std::ostream&
LoKi::MCParticles::IsParticle::fillStream
( std::ostream& s ) const 
{ return s << "ISMCPART" ; }
// ============================================================================
/*  constructor from contained object
 *  @param object object to be compared 
 */
// ============================================================================
LoKi::MCParticles::IsContainedObject::IsContainedObject
( const ContainedObject* object )
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate() 
  , m_objects ( 1 , object ) 
{}
// ============================================================================
/*  copy constructor
 *  @param right  object to be copied 
 */
// ============================================================================
LoKi::MCParticles::IsContainedObject::IsContainedObject
( const LoKi::MCParticles::IsContainedObject& right ) 
  : LoKi::AuxFunBase                        ( right )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate( right ) 
  , m_objects ( right.m_objects ) 
{}
// ============================================================================
//  destructor (virtual)
// ============================================================================
LoKi::MCParticles::IsContainedObject::~IsContainedObject() { m_objects.clear() ; }
// ============================================================================
//  MANDATORY: clone method ('virtual constructor')
// ============================================================================
LoKi::MCParticles::IsContainedObject*
LoKi::MCParticles::IsContainedObject::clone() const
{ return new LoKi::MCParticles::IsContainedObject( *this ) ; }
// ============================================================================
LoKi::MCParticles::IsContainedObject::result_type 
LoKi::MCParticles::IsContainedObject::operator() 
  ( LoKi::MCParticles::IsContainedObject::argument p ) const 
{
  Objects::const_iterator it = 
    std::find( m_objects.begin() , m_objects.end() , p ) ;
  return m_objects.end() != it ;
}
// ============================================================================
std::ostream&
LoKi::MCParticles::IsContainedObject::fillStream
( std::ostream& s ) const 
{ return s << "ISMCCO" ; }
// ============================================================================  
//  constructor
// ============================================================================  
LoKi::MCParticles::MomentumDistance::MomentumDistance
( const LoKi::LorentzVector& vct ) 
  : LoKi::AuxFunBase ( std::tie ( vct ) )  
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function() 
  , m_vct ( vct ) 
{}
// ============================================================================
// constructor from four components
// ============================================================================
LoKi::MCParticles::MomentumDistance::MomentumDistance
( const double px , 
  const double py , 
  const double pz , 
  const double e  ) 
  : LoKi::AuxFunBase ( std::tie ( px , py , pz , e ) ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function() 
  , m_vct () 
{
  m_vct.SetXYZT ( px , py , pz , e ) ;
}
// ============================================================================
//  copy constructor
// ============================================================================
LoKi::MCParticles::MomentumDistance::MomentumDistance
( const LoKi::MCParticles::MomentumDistance& right ) 
  : LoKi::AuxFunBase                       ( right )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function( right ) 
  , m_vct ( right.m_vct ) 
{}
// ============================================================================
//  MANDATORY::virtual destructor
// ============================================================================  
LoKi::MCParticles::MomentumDistance::~MomentumDistance(){}
// ============================================================================
//  MANDATORY: clone function ("virtual constructor")
// ============================================================================
LoKi::MCParticles::MomentumDistance*
LoKi::MCParticles::MomentumDistance::clone() const 
{ return new LoKi::MCParticles::MomentumDistance(*this) ; }
// ============================================================================
//  MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::MomentumDistance::result_type 
LoKi::MCParticles::MomentumDistance::operator() 
  ( LoKi::MCParticles::MomentumDistance::argument p ) const
{
  if ( 0 != p ) 
  { return LoKi::Kinematics::delta2euclidian ( p->momentum() , m_vct ) ; }
  Error ( "Invalid MCParticle*, return (InvalidMomentum)^2") ; 
  return 
    LoKi::Constants::InvalidMomentum * 
    LoKi::Constants::InvalidMomentum ;
}
// ============================================================================
//  "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::MCParticles::MomentumDistance::fillStream 
( std::ostream& s ) const 
{ return s << "MCMOMDIST*" 
           << m_vct.Px () << ","
           << m_vct.Py () << ","
           << m_vct.Pz () << ","
           << m_vct.E  () << ")" ; }
// ============================================================================
// constructor from theta & phi 
// ============================================================================
LoKi::MCParticles::TransverseMomentumRel::TransverseMomentumRel 
( const double theta , const double phi ) 
  : LoKi::AuxFunBase ( std::tie ( theta , phi ) ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function() 
  , m_vct () 
{
  m_vct.SetXYZ 
    ( ::sin ( theta ) * ::cos ( phi ) , 
      ::sin ( theta ) * ::sin ( phi ) , 
      ::cos ( theta )                 ) ;
}
// ============================================================================
/*  constructor
 *  @param vct direction vector 
 *  @see LoKi::ThreeVector 
 */
// ============================================================================
LoKi::MCParticles::TransverseMomentumRel::TransverseMomentumRel 
( const LoKi::ThreeVector& vct ) 
  : LoKi::AuxFunBase ( std::tie ( vct ) )  
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function() 
  , m_vct ( vct ) 
{}
// ============================================================================
//  copy constructor 
// ============================================================================
LoKi::MCParticles::TransverseMomentumRel::TransverseMomentumRel 
( const LoKi::MCParticles::TransverseMomentumRel& right ) 
  : LoKi::AuxFunBase                        ( right )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function( right ) 
  , m_vct ( right.m_vct ) 
{}
// ============================================================================
//  MANDATORY: virtual destructor
// ============================================================================
LoKi::MCParticles::TransverseMomentumRel::~TransverseMomentumRel(){}
// ============================================================================
//  MANDATORY: clone function ("virtual constructor")
// ============================================================================
LoKi::MCParticles::TransverseMomentumRel*
LoKi::MCParticles::TransverseMomentumRel::clone() const
{ return new LoKi::MCParticles::TransverseMomentumRel(*this) ; }
// ============================================================================
//  MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::TransverseMomentumRel::result_type 
LoKi::MCParticles::TransverseMomentumRel::operator() 
  ( LoKi::MCParticles::TransverseMomentumRel::argument p ) const
{
  if ( 0 != p ) 
  { return LoKi::Kinematics::transverseMomentumDir( p->momentum() , m_vct ) ; }
  Error ( "Invalid MCParticle*, return 'InvalidMomentum'") ; 
  return LoKi::Constants::InvalidMomentum ;
}
// ============================================================================
//  "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::MCParticles::TransverseMomentumRel::fillStream ( std::ostream& s ) const
{ return s << "MCPTDIR(" 
           << m_vct.Theta () << "," 
           << m_vct.Phi   () << ")" ; }
// ============================================================================
//  MANDATORY: virtual destructor 
// ============================================================================
LoKi::MCParticles::ValidOrigin::~ValidOrigin(){}
// ============================================================================
//  MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::ValidOrigin*
LoKi::MCParticles::ValidOrigin::clone() const 
{ return new LoKi::MCParticles::ValidOrigin(*this) ; }
// ============================================================================
//  MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::ValidOrigin::result_type 
LoKi::MCParticles::ValidOrigin::operator()
  ( LoKi::MCParticles::ValidOrigin::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ( " MCParticle* points to NULL, return 'false' ") ;
    return false ;
  };
  return 0 != p->originVertex() ;
}
// ============================================================================
//  "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::MCParticles::ValidOrigin::fillStream
( std::ostream& s ) const { return s << "MCOVALID" ; }
// ============================================================================
/*  constructor from vertex funtion 
 *  @param vfunc vertex function 
 *  @param err1  value to be returned 
 *         for invalid argument or 
 */
// ============================================================================
LoKi::MCParticles::MCVertexFunAdapter::MCVertexFunAdapter
( const LoKi::MCTypes::MCVFunc& vfunc , 
  const double                  err   ) 
  : LoKi::AuxFunBase ( std::tie ( vfunc , err ) ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function() 
  , m_fun ( vfunc ) 
  , m_err ( err   ) 
{}
// ============================================================================
//  MANDATORY : virtual destructor 
// ============================================================================
LoKi::MCParticles::MCVertexFunAdapter::~MCVertexFunAdapter(){}
// ============================================================================
//  MANDATORY : clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::MCVertexFunAdapter*
LoKi::MCParticles::MCVertexFunAdapter::clone() const 
{ return new LoKi::MCParticles::MCVertexFunAdapter(*this) ; }
// ============================================================================
//  MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::MCVertexFunAdapter::result_type 
LoKi::MCParticles::MCVertexFunAdapter::operator() 
  ( LoKi::MCParticles::MCVertexFunAdapter::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( " MCParticle* points to NULL, return 'm_err'") ;
    return m_err ;
  }
  //
  return m_fun ( p->originVertex() ) ;
}
// ============================================================================
//  "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::MCParticles::MCVertexFunAdapter::fillStream
( std::ostream& s ) const 
{ return s << "MCVFASPF(" << m_fun << ")" ; }
// ============================================================================
/*  constructor 
 *  @param decay  decay descriptor 
 *  @param finder decay finder tool 
 *  @see IMCDecayFinder
 */
// ============================================================================
LoKi::MCParticles::MCDecayPattern::MCDecayPattern 
( const std::string& decay  ,
  IMCDecayFinder*    finder ) 
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate()
  , m_finder ( finder ) 
  , m_decay  ( decay  ) 
{} 
// ============================================================================
/*  constructor 
 *  @param decay  decay descriptor 
 *  @param finder decay finder tool 
 *  @see IMCDecayFinder
 */
// ============================================================================
LoKi::MCParticles::MCDecayPattern::MCDecayPattern 
( const std::string&                     decay  ,
  const LoKi::Interface<IMCDecayFinder>& finder ) 
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate()
  , m_finder ( finder ) 
  , m_decay  ( decay  ) 
{} 
// ============================================================================
/*  copy constructor
 *  @param right object to be copied 
 */
// ============================================================================
LoKi::MCParticles::MCDecayPattern::MCDecayPattern 
( const LoKi::MCParticles::MCDecayPattern& right ) 
  : LoKi::AuxFunBase                        ( right )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate( right )
  , m_finder ( right.m_finder ) 
  , m_decay  ( right.m_decay  ) 
{} 
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::MCParticles::MCDecayPattern::~MCDecayPattern() {}
// ============================================================================
//  MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::MCDecayPattern*
LoKi::MCParticles::MCDecayPattern::clone() const 
{ return new LoKi::MCParticles::MCDecayPattern(*this) ; }
// ============================================================================
//  MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::MCDecayPattern::result_type 
LoKi::MCParticles::MCDecayPattern::operator() 
  ( LoKi::MCParticles::MCDecayPattern::argument p ) const 
{
  const LHCb::MCParticle* mc = p ;
  if ( 0 == p || 0 == mc ) 
  {
    Error ( " MCParticle* points to NULL, return 'false'") ;
    return false ;
  }
  if ( !m_finder.validPointer() ) 
  {
    Error ( " IMCDecayFinder* points to NULL, return 'false'") ;
    return false ;
  }
  if ( m_finder->decay() != m_decay ) 
  {
    StatusCode sc = m_finder->setDecay( m_decay ) ;
    if ( sc.isFailure() ) 
    {
      Error ( "Error from IMCDecayFinder::setDecay('"
              + m_decay + "') , return 'false' " , sc ) ;
      return false ;
    }
  }
  // prepare helper vector 
  std::vector<const LHCb::MCParticle*> vMC ( 1 , mc ) ;
  // use the tool 
  return m_finder->hasDecay( vMC ) ;
} 
// ============================================================================
//  "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::MCParticles::MCDecayPattern::fillStream
( std::ostream& s ) const 
{ 
  s << "MCDECAY(" ;
  if ( m_finder.validPointer() ) { s << "'" << m_finder->name() << "'," ; }  
  return s << Gaudi::Utils::toString ( m_decay ) << ")" ;
}
// ============================================================================
LoKi::MCParticles::MCFilter::MCFilter
( const IMCParticleSelector* selector ) 
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate ()
  , m_selector( selector ) 
{}
// ============================================================================
LoKi::MCParticles::MCFilter::MCFilter
( const LoKi::Interface<IMCParticleSelector>& selector ) 
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate ()
  , m_selector( selector ) 
{}
// ============================================================================
LoKi::MCParticles::MCFilter::MCFilter
( const LoKi::MCParticles::MCFilter& right ) 
  : LoKi::AuxFunBase                        ( right )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate ( right )
  , m_selector( right.m_selector ) 
{}
// ============================================================================
LoKi::MCParticles::MCFilter::~MCFilter(){}
// ============================================================================
LoKi::MCParticles::MCFilter*
LoKi::MCParticles::MCFilter::clone() const 
{ return new MCFilter(*this) ; }
// ============================================================================
LoKi::MCParticles::MCFilter::result_type
LoKi::MCParticles::MCFilter::operator()
  ( LoKi::MCParticles::MCFilter::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( " MCParticle* points to NULL, return 'false'") ;
    return false ;
  }
  if ( !m_selector.validPointer() ) 
  {
    Error ( " IMCParticleSelector* points to NULL, return 'false'") ;
    return false ;
  }
  // use the tool 
  return m_selector->accept ( p ) ;
}
// ============================================================================
std::ostream& 
LoKi::MCParticles::MCFilter::fillStream
( std::ostream& s ) const 
{ 
  s << "MCFILTER('" ;
  if ( m_selector.validPointer() ) 
  { s << m_selector->type() << "'/'" << m_selector->name() ; }
  return s << "')" ;
}
// ============================================================================
//  constructor from the phi 
// ============================================================================
LoKi::MCParticles::DeltaPhi::DeltaPhi ( const double phi )
  : LoKi::AuxFunBase ( std::tie ( phi ) ) 
  , LoKi::MCParticles::Phi ()  
  , m_phi  ( phi )
{ m_phi = adjust ( m_phi ) ; }
// ============================================================================
//  constructor from the vector 
// ============================================================================
LoKi::MCParticles::DeltaPhi::DeltaPhi ( const LoKi::ThreeVector& v ) 
  : LoKi::AuxFunBase ( std::tie ( v ) ) 
  , LoKi::MCParticles::Phi ()  
  , m_phi  ( v.Phi() )
{ m_phi = adjust ( m_phi ) ; }
// ============================================================================
//  constructor from the vector 
// ============================================================================
LoKi::MCParticles::DeltaPhi::DeltaPhi ( const LoKi::LorentzVector& v ) 
  : LoKi::AuxFunBase ( std::tie ( v ) ) 
  , LoKi::MCParticles::Phi ()  
  , m_phi  ( v.Phi() )
{ m_phi = adjust ( m_phi ) ; }
// ============================================================================
//  constructor from the particle
// ============================================================================
LoKi::MCParticles::DeltaPhi::DeltaPhi ( const LHCb::MCParticle* p )
  : LoKi::MCParticles::Phi ()  
  , m_phi  ( )
{ 
  if ( 0 == p ) { Exception("Invalid LHCb::MCParticle*") ;}
  m_phi = p->momentum().Phi() ;
  m_phi = adjust ( m_phi ) ; 
}  
// ============================================================================
//  MANDATORY: the only essential method 
// ============================================================================
LoKi::MCParticles::DeltaPhi::result_type
LoKi::MCParticles::DeltaPhi::operator() 
  ( LoKi::MCParticles::DeltaPhi::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ("LHCb::MCParticle* points to NULL, return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  return  dphi ( p ) ;
} 
// ============================================================================
//  OPTIONAL: "SHORT" representation
// ============================================================================
std::ostream& LoKi::MCParticles::DeltaPhi::fillStream ( std::ostream& s ) const 
{ return s << "MCDPHI(" << m_phi << ")" ; }
// ============================================================================
//  constructor from the eta 
// ============================================================================
LoKi::MCParticles::DeltaEta::DeltaEta ( const double eta  ) 
  : LoKi::AuxFunBase ( std::tie ( eta ) ) 
  , LoKi::MCParticles::PseudoRapidity ()  
  , m_eta  ( eta )
{}
// ============================================================================
//  constructor from the vector
// ============================================================================
LoKi::MCParticles::DeltaEta::DeltaEta ( const LoKi::ThreeVector& v ) 
  : LoKi::AuxFunBase ( std::tie ( v ) ) 
  , LoKi::MCParticles::PseudoRapidity ()  
  , m_eta  ( v.Eta () )
{}
// ============================================================================
//  constructor from the lorentz vector
// ============================================================================
LoKi::MCParticles::DeltaEta::DeltaEta ( const LoKi::LorentzVector& v ) 
  : LoKi::AuxFunBase ( std::tie ( v ) ) 
  , LoKi::MCParticles::PseudoRapidity ()  
  , m_eta  ( v.Eta () )
{}
// ============================================================================
//  constructor from the particle
// ============================================================================
LoKi::MCParticles::DeltaEta::DeltaEta ( const LHCb::MCParticle* p ) 
  : LoKi::MCParticles::PseudoRapidity () 
  , m_eta  ( )
{
  if ( 0 == p ) { Exception("Invalid LHCb::MCParticle*") ;}
  m_eta = p->momentum().Eta() ;
} 
// ============================================================================
//  MANDATORY: the only essential method 
// ============================================================================
LoKi::MCParticles::DeltaEta::result_type
LoKi::MCParticles::DeltaEta::operator() 
  ( LoKi::MCParticles::DeltaEta::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ("LHCb::MCParticle* points to NULL, return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  return deta ( p  ) ;
} 
// ============================================================================
//  OPTIONAL: "SHORT" representation
// ============================================================================
std::ostream& LoKi::MCParticles::DeltaEta::fillStream ( std::ostream& s ) const 
{ return s << "MCDETA(" << m_eta << ")" ; }
// ============================================================================
//  constructor from the eta and  phi
// ============================================================================
LoKi::MCParticles::DeltaR2::DeltaR2 
( const double eta , 
  const double phi ) 
  : LoKi::AuxFunBase ( std::tie ( eta , phi ) ) 
  , LoKi::MCParticles::DeltaPhi ( phi ) 
  , m_deta ( eta ) 
{}
// ============================================================================
//  constructor from the vector 
// ============================================================================
LoKi::MCParticles::DeltaR2::DeltaR2 ( const LoKi::ThreeVector& v ) 
  : LoKi::AuxFunBase ( std::tie ( v ) ) 
  , LoKi::MCParticles::DeltaPhi ( v ) 
  , m_deta ( v ) 
{}
// ============================================================================
//  constructor from the vector 
// ============================================================================
LoKi::MCParticles::DeltaR2::DeltaR2 ( const LoKi::LorentzVector& v ) 
  : LoKi::AuxFunBase ( std::tie ( v ) ) 
  , LoKi::MCParticles::DeltaPhi ( v ) 
  , m_deta ( v ) 
{}
// ============================================================================
//  constructor from the particle
// ============================================================================
LoKi::MCParticles::DeltaR2::DeltaR2 ( const LHCb::MCParticle* v ) 
  : LoKi::MCParticles::DeltaPhi ( v ) 
  , m_deta ( v ) 
{}
// ============================================================================
//  MANDATORY: the only essential method 
// ============================================================================
LoKi::MCParticles::DeltaR2::result_type
LoKi::MCParticles::DeltaR2::operator() 
  ( LoKi::MCParticles::DeltaR2::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ("LHCb::MCParticle* points to NULL, return 'InvalidDistance'") ;
    return LoKi::Constants::InvalidDistance;
  }
  const double dphi_ = dphi ( p ) ;
  const double deta_ = deta ( p ) ;
  //
  return dphi_*dphi_ + deta_*deta_ ;
} 
// ============================================================================
//  OPTIONAL: "SHORT" representation
// ============================================================================
std::ostream& LoKi::MCParticles::DeltaR2::fillStream ( std::ostream& s ) const 
{ return s << "MCDR2(" << eta0() << "," << phi0() << ")" ; }
// ============================================================================
// constructor form the tool 
// ============================================================================
LoKi::MCParticles::MCReconstructible::MCReconstructible
( const IMCReconstructible* tool ) 
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Function() 
  , m_eval ( tool ) 
{}
// ============================================================================
// constructor form the tool 
// ============================================================================
LoKi::MCParticles::MCReconstructible::MCReconstructible
( const LoKi::Interface<IMCReconstructible>& tool ) 
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Function() 
  , m_eval ( tool ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::MCParticles::MCReconstructible::MCReconstructible
( const LoKi::MCParticles::MCReconstructible& right )
  : LoKi::AuxFunBase ( right )  
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function( right )  
  , m_eval ( right.m_eval ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::MCReconstructible*
LoKi::MCParticles::MCReconstructible::clone() const
{ return new  LoKi::MCParticles::MCReconstructible ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::MCReconstructible::result_type
LoKi::MCParticles::MCReconstructible::operator() 
  ( LoKi::MCParticles::MCReconstructible::argument p ) const 
{
  static const IMCReconstructible::RecCategory s_bad = 
    IMCReconstructible::NoClassification ;
  //
  if ( 0 == p ) 
  {
    Error ( "LHCb::MCParticle* points to NULL, return '" + 
            IMCReconstructible::text ( s_bad ) + "'" ) ;
    return s_bad ;
  }
  if ( !m_eval.validPointer() ) 
  {
    Error ( "IMCRecontructible* points to NULL, return '" + 
            IMCReconstructible::text ( s_bad ) + "'" ) ;
    return s_bad ;
  }
  // use the tool 
  return m_eval->reconstructible( p ) ; ///< use the tool 
}
// ============================================================================
// OPTIONAL: "short representation"
// ============================================================================
std::ostream& 
LoKi::MCParticles::MCReconstructible::fillStream( std::ostream& s ) const
{ return s << "MCREC('" << m_eval->name() << "')" ; }
// ============================================================================
// constructor from the tool and category 
// ============================================================================
LoKi::MCParticles::MCReconstructibleAs::MCReconstructibleAs 
( const IMCReconstructible*             tool , 
  const IMCReconstructible::RecCategory cat  ) 
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate () 
  , m_eval ( tool ) 
  , m_cat ( cat )
{}
// ============================================================================
// constructor from the tool and category 
// ============================================================================
LoKi::MCParticles::MCReconstructibleAs::MCReconstructibleAs 
( const IMCReconstructible::RecCategory cat  ,
  const IMCReconstructible*             tool ) 
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate () 
  , m_eval ( tool ) 
  , m_cat ( cat )
{}
// ============================================================================
// constructor from the holder and category
// ============================================================================
LoKi::MCParticles::MCReconstructibleAs::MCReconstructibleAs
( const LoKi::Interface<IMCReconstructible>& tool , 
  const IMCReconstructible::RecCategory      cat  ) 
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate () 
  , m_eval ( tool ) 
  , m_cat ( cat )
{}
// ============================================================================
// constructor from the holder and category
// ============================================================================
LoKi::MCParticles::MCReconstructibleAs::MCReconstructibleAs
( const IMCReconstructible::RecCategory      cat  ,
  const LoKi::Interface<IMCReconstructible>& tool ) 
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate () 
  , m_eval ( tool ) 
  , m_cat ( cat )
{}
// ============================================================================
// constructor from the holder and category
// ============================================================================
LoKi::MCParticles::MCReconstructibleAs::MCReconstructibleAs
( const LoKi::MCParticles::MCReconstructibleAs& right ) 
  : LoKi::AuxFunBase ( right )  
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate( right )  
  , m_eval ( right.m_eval ) 
  , m_cat  ( right.m_cat  ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::MCReconstructibleAs*
LoKi::MCParticles::MCReconstructibleAs::clone() const 
{ return new LoKi::MCParticles::MCReconstructibleAs(*this); }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::MCParticles::MCReconstructibleAs::result_type 
LoKi::MCParticles::MCReconstructibleAs::operator() 
  ( LoKi::MCParticles::MCReconstructibleAs::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ("LHCb::MCParticle* points to NULL, return 'False'") ;
    return false ;
  }
  //
  if ( !m_eval.validPointer() ) 
  {
    Error ( "IMCRecontructible* points to NULL, return 'False'" ) ;
    return false ;
  } 
  // use the tool!
  return m_cat == m_eval->reconstructible( p ) ; ///< use the tool 
}
// ============================================================================
// OPTIONAL: "short representation"
// ============================================================================
std::ostream& 
LoKi::MCParticles::MCReconstructibleAs::fillStream( std::ostream& s ) const 
{ return s << "MCRECAS[\"" << IMCReconstructible::text ( m_cat ) << "\"]" ; }
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::MCParticles::ChildFunction::ChildFunction 
( const LoKi::MCTypes::MCFunc& fun   , 
  const size_t                 index ,
  const double                 bad   ) 
  : LoKi::AuxFunBase ( std::tie ( fun , index , bad ) ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function() 
  , m_fun   ( fun   ) 
  , m_index ( index ) 
  , m_bad   ( bad   ) 
{}
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 */
// ============================================================================
LoKi::MCParticles::ChildFunction::ChildFunction 
( const size_t                 index ,
  const LoKi::MCTypes::MCFunc& fun   ,
  const double                 bad   ) 
  : LoKi::AuxFunBase ( std::tie ( index , fun , bad ) ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function() 
  , m_fun   ( fun   ) 
  , m_index ( index ) 
  , m_bad   ( bad   ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::MCParticles::ChildFunction::ChildFunction
( const LoKi::MCParticles::ChildFunction& right ) 
  : LoKi::AuxFunBase ( right ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function( right ) 
  , m_fun   ( right.m_fun   ) 
  , m_index ( right.m_index ) 
  , m_bad   ( right.m_bad   ) 
{}
// ============================================================================
//  MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::ChildFunction* 
LoKi::MCParticles::ChildFunction::clone() const 
{ return new LoKi::MCParticles::ChildFunction ( *this ) ; }
// ============================================================================
//  MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::ChildFunction::result_type 
LoKi::MCParticles::ChildFunction::operator() 
  ( LoKi::MCParticles::ChildFunction::argument p ) const 
{
  if ( 0 == p && 0 != m_index ) 
  { 
    Error ( "LHCb::MCParticle* points to NULL, return " + 
            LoKi::Print::toString( m_bad ) ) ;
    return m_bad ;
  }
  // extract the daughter 
  const LHCb::MCParticle* daughter = LoKi::Child::child ( p , m_index ) ;
  if ( 0 == daughter ) { Warning ( "Daughter particle points to NULL" ) ; }
  return m_fun ( daughter ) ;
}
// ============================================================================
// OPTIONAL:  specific printout 
// ============================================================================
std::ostream& LoKi::MCParticles::ChildFunction::fillStream 
( std::ostream& s ) const 
{
  s << "MCCHILD(" 
    << m_fun 
    << "," << m_index ;
  if ( LoKi::Constants::NegativeInfinity != m_bad ) { s << "," << m_bad ; }
  return s << ")" ;
}
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param fun    the function to be used 
 *  @param index  the index of daughter particle
 *  @param bad    the return valeu for invalid particle 
 */
// ============================================================================
LoKi::MCParticles::ChildPredicate::ChildPredicate 
( const LoKi::MCTypes::MCCuts& cut   , 
  const size_t                 index ,
  const bool                   bad   ) 
  : LoKi::AuxFunBase ( std::tie ( cut , index , bad ) ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate ()
  , m_cut   ( cut   )
  , m_index ( index ) 
  , m_bad   ( bad   )
{}
// ============================================================================
/*  constructor from the function and daughter index 
 *  @param index  the index of daughter particle
 *  @param fun    the function to be used 
 *  @param bad    the return valeu for invalid particle 
 */
// ============================================================================
LoKi::MCParticles::ChildPredicate::ChildPredicate 
( const size_t                 index ,
  const LoKi::MCTypes::MCCuts& cut   , 
  const bool                   bad   ) 
  : LoKi::AuxFunBase ( std::tie ( index , cut , index ) ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate ()
  , m_cut   ( cut   )
  , m_index ( index ) 
  , m_bad   ( bad   )
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::MCParticles::ChildPredicate::ChildPredicate 
( const LoKi::MCParticles::ChildPredicate& right ) 
  : LoKi::AuxFunBase ( right ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate ( right )
  , m_cut   ( right.m_cut   )
  , m_index ( right.m_index ) 
  , m_bad   ( right.m_bad   )
{}
// ============================================================================
//  MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::ChildPredicate* 
LoKi::MCParticles::ChildPredicate::clone() const 
{ return new LoKi::MCParticles::ChildPredicate(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::ChildPredicate::result_type 
LoKi::MCParticles::ChildPredicate::operator()  
  ( LoKi::MCParticles::ChildPredicate::argument p ) const 
{
  if ( 0 == p && 0 != m_index ) 
  { 
    Error ( "LHCb::MCParticle* points to NULL, return " + 
            LoKi::Print::toString( m_bad ) ) ;
    return m_bad ;
  }
  // extract the daughter 
  const LHCb::MCParticle* daughter = LoKi::Child::child ( p , m_index ) ;
  if ( 0 == daughter ) { Warning ( "Daughter particle points to NULL" ) ; }
  return m_cut ( daughter ) ;
}
// ============================================================================
// OPTIONAL:  specific printout 
// ============================================================================
std::ostream& LoKi::MCParticles::ChildPredicate::fillStream 
( std::ostream& s ) const 
{
  s << "MCCHILDCUT(" 
    << m_cut
    << "," << m_index ;
  if ( false != m_bad ) { s << "," << LoKi::Print::toString ( m_bad )  ; }
  return s << ")" ;
}
// ============================================================================
/*  standard constructor 
 *  @param cut cut to be checked 
 *  @param decayOnly flag to indicat the search through decay products only
 */
// ============================================================================
LoKi::MCParticles::InTree::InTree  
( const LoKi::MCTypes::MCCuts& cut       , 
  const bool                   decayOnly ) 
  : LoKi::AuxFunBase ( std::tie ( cut , decayOnly  ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate () 
  , m_cut       ( cut       ) 
  , m_decayOnly ( decayOnly ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::MCParticles::InTree::InTree  
( const LoKi::MCParticles::InTree& right ) 
  : LoKi::AuxFunBase ( right ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate ( right ) 
  , m_cut       ( right.m_cut       ) 
  , m_decayOnly ( right.m_decayOnly ) 
{}
// ============================================================================
// MANDATORY: clone function ("virtual constructor")
// ============================================================================
LoKi::MCParticles::InTree*
LoKi::MCParticles::InTree::clone() const 
{ return new LoKi::MCParticles::InTree(*this); }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::InTree::result_type 
LoKi::MCParticles::InTree::operator()
  ( LoKi::MCParticles::InTree::argument p ) const 
{
  if ( 0 == p ) { Warning ("LHCb::MCParticle* points to NULL" ) ; }
  return LoKi::MCAlgs::found ( p , m_cut , m_decayOnly ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::MCParticles::InTree::fillStream ( std::ostream& s ) const 
{
  return s << "MCINTREE(" << LoKi::Print::print ( m_cut ) 
           << ","         << LoKi::Print::print ( m_decayOnly ) << ")" ; 
}
// ============================================================================
/*  standard constructor 
 *  @param cut cut to be checked 
 *  @param decayOnly flag to indicat the search through decay products only
 */
// ============================================================================
LoKi::MCParticles::NinTree::NinTree  
( const LoKi::MCTypes::MCCuts& cut       , 
  const bool                   decayOnly ) 
  : LoKi::AuxFunBase ( std::tie ( cut , decayOnly  ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function () 
  , m_cut       ( cut       ) 
  , m_decayOnly ( decayOnly ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::MCParticles::NinTree::NinTree  
( const LoKi::MCParticles::NinTree& right ) 
  : LoKi::AuxFunBase ( right ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function ( right ) 
  , m_cut       ( right.m_cut       ) 
  , m_decayOnly ( right.m_decayOnly ) 
{}
// ============================================================================
// MANDATORY: clone function ("virtual constructor")
// ============================================================================
LoKi::MCParticles::NinTree*
LoKi::MCParticles::NinTree::clone() const 
{ return new LoKi::MCParticles::NinTree(*this); }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::NinTree::result_type 
LoKi::MCParticles::NinTree::operator()
  ( LoKi::MCParticles::NinTree::argument p ) const 
{
  if ( 0 == p ) { Warning ("LHCb::MCParticle* points to NULL" ) ; }
  return LoKi::MCAlgs::count_if ( p , m_cut , m_decayOnly ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::MCParticles::NinTree::fillStream ( std::ostream& s ) const 
{
  return s << "MCNINTREE(" << LoKi::Print::print ( m_cut ) 
           << ","          << LoKi::Print::print ( m_decayOnly ) << ")" ; 
}
// ============================================================================
/*  standard constructor 
 *  @param fun the function to be evaluated 
 *  @param cut cut to be applied 
 *  @param decayObnly flag to traverse only decay tree 
 *  @param res initial value 
 */
// ============================================================================
LoKi::MCParticles::SumTree::SumTree  
( const LoKi::MCTypes::MCFunc& fun       , 
  const LoKi::MCTypes::MCCuts& cut       , 
  const bool                   decayOnly , 
  const double                 result    ) 
  : LoKi::AuxFunBase ( std::tie ( fun , cut , decayOnly  , result ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function () 
  , m_fun       ( fun       ) 
  , m_cut       ( cut       ) 
  , m_decayOnly ( decayOnly ) 
  , m_res       ( result    ) 
{}
// ============================================================================
/*  standard constructor 
 *  @param fun the function to be evaluated 
 *  @param cut cut to be applied 
 *  @param decayObnly flag to traverse only decay tree 
 *  @param res initial value 
 */
// ============================================================================
LoKi::MCParticles::SumTree::SumTree  
( const LoKi::MCTypes::MCCuts& cut       ,
  const LoKi::MCTypes::MCFunc& fun       , 
  const bool                   decayOnly , 
  const double                 result    ) 
  : LoKi::AuxFunBase ( std::tie ( cut , fun , decayOnly  , result ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function () 
  , m_fun       ( fun       ) 
  , m_cut       ( cut       ) 
  , m_decayOnly ( decayOnly ) 
  , m_res       ( result    ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::MCParticles::SumTree::SumTree  
( const LoKi::MCParticles::SumTree& right ) 
  : LoKi::AuxFunBase ( right ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function ( right ) 
  , m_fun       ( right.m_fun       ) 
  , m_cut       ( right.m_cut       ) 
  , m_decayOnly ( right.m_decayOnly ) 
  , m_res       ( right.m_res       ) 
{}
// ============================================================================
// MANDATORY: clone function ("virtual constructor")
// ============================================================================
LoKi::MCParticles::SumTree*
LoKi::MCParticles::SumTree::clone() const 
{ return new LoKi::MCParticles::SumTree(*this); }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::SumTree::result_type 
LoKi::MCParticles::SumTree::operator()
  ( LoKi::MCParticles::SumTree::argument p ) const 
{
  if ( 0 == p ) { Warning ("LHCb::MCParticle* points to NULL" ) ; }
  return LoKi::MCAlgs::accumulate 
    ( p , m_fun , m_cut , m_res ,  std::plus<result_type>() , m_decayOnly ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::MCParticles::SumTree::fillStream ( std::ostream& s ) const 
{
  s << "MCSUMTREE("   << m_fun   
    << ","            << m_cut  
    << ","            << LoKi::Print::print ( m_decayOnly ) ;
  if ( 0 != m_res ) 
  { s << ","          << LoKi::Print::print ( m_res  ) ; }
  return s << ")" ;
}
// ============================================================================
/*  standard constructor 
 *  @param fun the function to be evaluated 
 *  @param cut cut to be applied 
 *  @param decayObnly flag to traverse only decay tree 
 *  @param res initial value 
 */
// ============================================================================
LoKi::MCParticles::MultTree::MultTree  
( const LoKi::MCTypes::MCFunc& fun       , 
  const LoKi::MCTypes::MCCuts& cut       , 
  const bool                   decayOnly , 
  const double                 result    ) 
  : LoKi::AuxFunBase ( std::tie ( fun , cut , decayOnly  , result ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function () 
  , m_fun       ( fun       ) 
  , m_cut       ( cut       ) 
  , m_decayOnly ( decayOnly ) 
  , m_res       ( result    ) 
{}
// ============================================================================
/*  standard constructor 
 *  @param fun the function to be evaluated 
 *  @param cut cut to be applied 
 *  @param decayObnly flag to traverse only decay tree 
 *  @param res initial value 
 */
// ============================================================================
LoKi::MCParticles::MultTree::MultTree  
( const LoKi::MCTypes::MCCuts& cut       ,
  const LoKi::MCTypes::MCFunc& fun       , 
  const bool                   decayOnly , 
  const double                 result    ) 
  : LoKi::AuxFunBase ( std::tie ( cut , fun , decayOnly  , result ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function () 
  , m_fun       ( fun       ) 
  , m_cut       ( cut       ) 
  , m_decayOnly ( decayOnly ) 
  , m_res       ( result    ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::MCParticles::MultTree::MultTree  
( const LoKi::MCParticles::MultTree& right ) 
  : LoKi::AuxFunBase ( right ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function ( right ) 
  , m_fun       ( right.m_fun       ) 
  , m_cut       ( right.m_cut       ) 
  , m_decayOnly ( right.m_decayOnly ) 
  , m_res       ( right.m_res       ) 
{}
// ============================================================================
// MANDATORY: clone function ("virtual constructor")
// ============================================================================
LoKi::MCParticles::MultTree*
LoKi::MCParticles::MultTree::clone() const 
{ return new LoKi::MCParticles::MultTree(*this); }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::MultTree::result_type 
LoKi::MCParticles::MultTree::operator()
  ( LoKi::MCParticles::MultTree::argument p ) const 
{
  if ( 0 == p ) { Warning ("LHCb::MCParticle* points to NULL" ) ; }
  return LoKi::MCAlgs::accumulate 
    ( p , m_fun , m_cut , m_res ,  std::multiplies<result_type>() , m_decayOnly ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::MCParticles::MultTree::fillStream ( std::ostream& s ) const 
{
  s << "MCMULTTREE("   << m_fun   
    << ","             << m_cut  
    << ","             << LoKi::Print::print ( m_decayOnly ) ;
  if ( 1 != m_res ) 
  { s << ","           << LoKi::Print::print ( m_res  ) ; }
  return s << ")" ;
}
// ============================================================================
/*  standard constructor 
 *  @param fun the function to be evaluated 
 *  @param cut cut to be applied 
 *  @param decayObnly flag to traverse only decay tree 
 *  @param res initial value 
 */
// ============================================================================
LoKi::MCParticles::MinTree::MinTree  
( const LoKi::MCTypes::MCFunc& fun       , 
  const LoKi::MCTypes::MCCuts& cut       , 
  const bool                   decayOnly , 
  const double                 result    ) 
  : LoKi::AuxFunBase ( std::tie ( fun , cut , decayOnly  , result ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function () 
  , m_fun       ( fun       ) 
  , m_cut       ( cut       ) 
  , m_decayOnly ( decayOnly ) 
  , m_res       ( result    ) 
{}
// ============================================================================
/*  standard constructor 
 *  @param fun the function to be evaluated 
 *  @param cut cut to be applied 
 *  @param decayObnly flag to traverse only decay tree 
 *  @param res initial value 
 */
// ============================================================================
LoKi::MCParticles::MinTree::MinTree  
( const LoKi::MCTypes::MCCuts& cut       ,
  const LoKi::MCTypes::MCFunc& fun       , 
  const bool                   decayOnly , 
  const double                 result    ) 
  : LoKi::AuxFunBase ( std::tie ( cut , fun , decayOnly  , result ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function () 
  , m_fun       ( fun       ) 
  , m_cut       ( cut       ) 
  , m_decayOnly ( decayOnly ) 
  , m_res       ( result    ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::MCParticles::MinTree::MinTree  
( const LoKi::MCParticles::MinTree& right ) 
  : LoKi::AuxFunBase ( right ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function ( right ) 
  , m_fun       ( right.m_fun       ) 
  , m_cut       ( right.m_cut       ) 
  , m_decayOnly ( right.m_decayOnly ) 
  , m_res       ( right.m_res       ) 
{}
// ============================================================================
// MANDATORY: clone function ("virtual constructor")
// ============================================================================
LoKi::MCParticles::MinTree*
LoKi::MCParticles::MinTree::clone() const 
{ return new LoKi::MCParticles::MinTree(*this); }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::MinTree::result_type 
LoKi::MCParticles::MinTree::operator()
  ( LoKi::MCParticles::MinTree::argument p ) const 
{
  if ( 0 == p ) { Warning ("LHCb::MCParticle* points to NULL" ) ; }
  return LoKi::MCAlgs::accumulate 
    ( p , m_fun , m_cut , m_res ,  std::multiplies<result_type>() , m_decayOnly ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::MCParticles::MinTree::fillStream ( std::ostream& s ) const 
{
  s << "MCMINTREE("   << m_fun   
    << ","            << m_cut  
    << ","            << LoKi::Print::print ( m_decayOnly ) ;
  if ( LoKi::Constants::PositiveInfinity != m_res ) 
  { s << ","          << LoKi::Print::print ( m_res  ) ; }
  return s << ")" ;
}
// ============================================================================
/*  standard constructor 
 *  @param fun the function to be evaluated 
 *  @param cut cut to be applied 
 *  @param decayObnly flag to traverse only decay tree 
 *  @param res initial value 
 */
// ============================================================================
LoKi::MCParticles::MaxTree::MaxTree  
( const LoKi::MCTypes::MCFunc& fun       , 
  const LoKi::MCTypes::MCCuts& cut       , 
  const bool                   decayOnly , 
  const double                 result    ) 
  : LoKi::AuxFunBase ( std::tie ( fun , cut , decayOnly  , result ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function () 
  , m_fun       ( fun       ) 
  , m_cut       ( cut       ) 
  , m_decayOnly ( decayOnly ) 
  , m_res       ( result    ) 
{}
// ============================================================================
/*  standard constructor 
 *  @param fun the function to be evaluated 
 *  @param cut cut to be applied 
 *  @param decayObnly flag to traverse only decay tree 
 *  @param res initial value 
 */
// ============================================================================
LoKi::MCParticles::MaxTree::MaxTree  
( const LoKi::MCTypes::MCCuts& cut       ,
  const LoKi::MCTypes::MCFunc& fun       , 
  const bool                   decayOnly , 
  const double                 result    ) 
  : LoKi::AuxFunBase ( std::tie ( cut , fun , decayOnly  , result ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function () 
  , m_fun       ( fun       ) 
  , m_cut       ( cut       ) 
  , m_decayOnly ( decayOnly ) 
  , m_res       ( result    ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::MCParticles::MaxTree::MaxTree  
( const LoKi::MCParticles::MaxTree& right ) 
  : LoKi::AuxFunBase ( right ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function ( right ) 
  , m_fun       ( right.m_fun       ) 
  , m_cut       ( right.m_cut       ) 
  , m_decayOnly ( right.m_decayOnly ) 
  , m_res       ( right.m_res       ) 
{}
// ============================================================================
// MANDATORY: clone function ("virtual constructor")
// ============================================================================
LoKi::MCParticles::MaxTree*
LoKi::MCParticles::MaxTree::clone() const 
{ return new LoKi::MCParticles::MaxTree(*this); }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::MaxTree::result_type 
LoKi::MCParticles::MaxTree::operator()
  ( LoKi::MCParticles::MaxTree::argument p ) const 
{
  if ( 0 == p ) { Warning ("LHCb::MCParticle* points to NULL" ) ; }
  return LoKi::MCAlgs::accumulate 
    ( p , m_fun , m_cut , m_res ,  std::multiplies<result_type>() , m_decayOnly ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::MCParticles::MaxTree::fillStream ( std::ostream& s ) const 
{
  s << "MCMAXTREE("   << m_fun   
    << ","            << m_cut  
    << ","            << LoKi::Print::print ( m_decayOnly ) ;
  if ( LoKi::Constants::NegativeInfinity != m_res ) 
  { s << ","          << LoKi::Print::print ( m_res  ) ; }
  return s << ")" ;
}

// ============================================================================
// constructor from the predicate 
// ============================================================================
LoKi::MCParticles::NinAncestors::NinAncestors 
( const LoKi::MCTypes::MCCuts& cuts )
  : LoKi::AuxFunBase ( std::tie ( cuts ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function ()
  , m_cut ( cuts )
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::MCParticles::NinAncestors::~NinAncestors(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::NinAncestors*
LoKi::MCParticles::NinAncestors::clone() const
{ return new LoKi::MCParticles::NinAncestors ( *this ) ; }
// ============================================================================
// MANDATORY: the only one important method 
// ============================================================================
LoKi::MCParticles::NinAncestors::result_type 
LoKi::MCParticles::NinAncestors::operator() 
  ( LoKi::MCParticles::InAncestors::argument p ) const 
{ 
  return nInAncestors ( p ) ; 
}
// ============================================================================
// the actual evaluator 
// ============================================================================
int LoKi::MCParticles::NinAncestors::nInAncestors 
( const LHCb::MCParticle* p ) const 
{ 
  if ( 0 == p )
  {
    Error ( "LHCb::MCParticle* point to NULL, return 0") ;
    return 0 ;
  }
  int found = 0 ;
  const LHCb::MCParticle* mother = p->mother() ;
  while ( 0 != mother ) 
  {
    if ( m_cut ( mother ) ) { return ++found  ; }   // COUNT 
    mother = mother->mother() ;
  }
  return found ;
}
// ============================================================================
// OPTIONAL: "SHORT" representation
// ============================================================================
std::ostream& 
LoKi::MCParticles::NinAncestors::fillStream( std::ostream& s ) const 
{ return s << " MCNINANCESTORS( " << m_cut << " ) " ; }

// ============================================================================
// constructor from the predicate 
// ============================================================================
LoKi::MCParticles::InAncestors::InAncestors 
( const LoKi::MCTypes::MCCuts& cuts ) 
  : LoKi::AuxFunBase ( std::tie ( cuts ) )
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate ()
  , m_cut ( cuts )
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::MCParticles::InAncestors::~InAncestors(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::InAncestors*
LoKi::MCParticles::InAncestors::clone() const
{ return new LoKi::MCParticles::InAncestors ( *this ) ; }
// ============================================================================
// MANDATORY: the only one important method 
// ============================================================================
LoKi::MCParticles::InAncestors::result_type 
LoKi::MCParticles::InAncestors::operator() 
  ( LoKi::MCParticles::InAncestors::argument p ) const 
{ 
  return inAncestors ( p ) ; 
}
// ============================================================================
// the actual evaluator 
// ============================================================================
bool LoKi::MCParticles::InAncestors::inAncestors 
( const LHCb::MCParticle* p ) const 
{ 
  if ( 0 == p )
  {
    Error ( "LHCb::MCParticle* point to NULL, return false") ;
    return false ;
  }
  const LHCb::MCParticle* mother = p->mother() ;
  while ( 0 != mother ) 
  {
    if ( m_cut ( mother ) ) { return true  ; }   // RETURN 
    //
    mother = mother->mother() ;                  //  
  }
  return false ;
}
// ============================================================================
// OPTIONAL: "SHORT" representation
// ============================================================================
std::ostream& 
LoKi::MCParticles::InAncestors::fillStream( std::ostream& s ) const 
{ return s << " MCINANCESTORS( " << m_cut << " ) " ; }
// ============================================================================


// ============================================================================
// MANDATORY: virtual desructor 
// ============================================================================
LoKi::MCParticles::FromDecays::~FromDecays(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::FromDecays*
LoKi::MCParticles::FromDecays::clone () const
{ return new LoKi::MCParticles::FromDecays ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::MCParticles::FromDecays::result_type 
LoKi::MCParticles::FromDecays::operator() 
  ( LoKi::MCParticles::FromDecays::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::MCParticle* points to NULL, return false") ;
    return false ;
  }
  const LHCb::MCVertex* vertex = p->originVertex() ;
  if ( 0 == vertex ) { return false ; }               // RETURN
  //
  while ( 0 != vertex ) 
  {
    switch ( vertex->type() ) 
    {
    case LHCb::MCVertex::ppCollision        : break ;
    case LHCb::MCVertex::DecayVertex        : break ;
    case LHCb::MCVertex::OscillatedAndDecay : break ;
    default:                                  return false ; // RETURN
    }
    const LHCb::MCParticle* mother = vertex->mother() ;
    if ( 0 != mother ) { vertex = mother->originVertex() ; }
    else               { vertex = 0 ; }
    // 
  }
  //
  return true ;
}
// ============================================================================
// OPTIONAL: "SHORT" representation
// ============================================================================
std::ostream& 
LoKi::MCParticles::FromDecays::fillStream( std::ostream& s ) const 
{ return s << " MCFROMDECAYS " ; }
// ============================================================================



// ============================================================================
// MANDATORY: virtual desructor 
// ============================================================================
LoKi::MCParticles::FromInteractions::~FromInteractions(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::FromInteractions*
LoKi::MCParticles::FromInteractions::clone () const
{ return new LoKi::MCParticles::FromInteractions ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::MCParticles::FromInteractions::result_type 
LoKi::MCParticles::FromInteractions::operator() 
  ( LoKi::MCParticles::FromInteractions::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::MCParticle* points to NULL, return false") ;
    return false ;
  }
  const LHCb::MCVertex* vertex = p->originVertex() ;
  if ( 0 == vertex ) { return false ; }               // RETURN
  //
  while ( 0 != vertex ) 
  {
    switch ( vertex->type() ) 
    {
    case LHCb::MCVertex::ppCollision        : break ;
    case LHCb::MCVertex::DecayVertex        : break ;
    case LHCb::MCVertex::OscillatedAndDecay : break ;
    default:                                  return true ; // RETURN
    }
    const LHCb::MCParticle* mother = vertex->mother() ;
    if ( 0 != mother ) { vertex = mother->originVertex() ; }
    else               { vertex = 0 ; }
    // 
  }
  //
  return false ;
}
// ============================================================================
// OPTIONAL: "SHORT" representation
// ============================================================================
std::ostream& 
LoKi::MCParticles::FromInteractions::fillStream ( std::ostream& s ) const 
{ return s << " MCFROMXS" ; }
// ============================================================================

// ============================================================================
// get unique string for HepMC::Particle 
// ============================================================================
std::string LoKi::MCParticles::hex_id ( const LHCb::MCParticle* particle ) 
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
