// $Id: Particles0.cpp,v 1.20 2010-01-17 16:48:42 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL
// ============================================================================
#include <algorithm>
#include <cmath>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ToStream.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/ParticleProperties.h"
#include "LoKi/Constants.h"
#include "LoKi/Report.h"
#include "LoKi/Kinematics.h"
#include "LoKi/Services.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Particles0.h"
#include "LoKi/Child.h"
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
LoKi::Particles::HasKey::result_type 
LoKi::Particles::HasKey::operator() 
  ( LoKi::Particles::HasKey::argument p ) const 
{ return 0 == p ? false : p->hasKey() ; }
// ============================================================================
std::ostream& 
LoKi::Particles::HasKey::fillStream ( std::ostream& s ) const 
{ return s << "HASKEY" ; }
// ============================================================================
LoKi::Particles::Key::Key
( const LHCb::Particle::key_type bad ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , m_bad   ( bad ) 
  , m_nokey ( bad ) 
{} 
// ============================================================================
LoKi::Particles::Key::Key
( const LHCb::Particle::key_type bad   ,
  const LHCb::Particle::key_type nokey ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , m_bad   ( bad   ) 
  , m_nokey ( nokey ) 
{} 
// ============================================================================
LoKi::Particles::Key::Key
( const LoKi::Particles::Key& right ) 
  : LoKi::AuxFunBase                      ( right ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function ( right ) 
  , m_bad   ( right.m_bad   ) 
  , m_nokey ( right.m_nokey ) 
{} 
// ============================================================================
LoKi::Particles::Key::result_type
LoKi::Particles::Key::operator() 
  ( LoKi::Particles::Key::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error(" Argument is invalid! return " + 
          Gaudi::Utils::toString ( m_bad )  ) ;
    return m_bad ;
  }
  if ( !p->hasKey() ) 
  {
    Error(" Key is not set! return " + 
          Gaudi::Utils::toString ( m_nokey ) ) ;
    return m_nokey ;
  }
  return result_type( p->key() ) ;
} 
// ============================================================================
std::ostream& 
LoKi::Particles::Key::fillStream ( std::ostream& s ) const 
{ return s << "KEY" ; }
// ============================================================================
LoKi::Particles::InTES::InTES 
( const std::string& path , 
  const bool         full ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate ()
  , m_location ( path ) 
  , m_fullpath ( full ) 
{} 
// ============================================================================
LoKi::Particles::InTES::InTES 
( const LoKi::Particles::InTES& right ) 
  : LoKi::AuxFunBase                       ( right ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate ( right )
  , m_location ( right.m_location ) 
  , m_fullpath ( right.m_fullpath ) 
{}
// ============================================================================
LoKi::Particles::InTES::result_type 
LoKi::Particles::InTES::operator() 
  ( LoKi::Particles::InTES::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error(" Argument is invalid! return false" ) ;
    return false ;                                              // RETURN 
  }
  //
  const DataObject* obj = p->parent() ;
  if ( 0 == obj )               { return false ; }             // RETURN 
  //
  const IRegistry* reg = obj->registry() ;
  if ( 0 == reg )               { return false ; }             // RETURN 
  //
  const std::string& path = reg->identifier() ;
  //
  if ( fullpath() ) 
  {
    //
    if ( !location().empty() && '/' != location()[0] ) 
    {
      if ( path == "/Event/"      + location() ) { return true ; } // RETURN 
      if ( path == "/Event/Phys/" + location() ) { return true ; } // RETURN
    }
    // compare fullpath 
    return path == location() ;                                   // RETURN 
  }
  // search for a substring:
  return std::string::npos != path.find( location() ) ;
} ;
// ============================================================================
std::ostream& 
LoKi::Particles::InTES::fillStream ( std::ostream& s ) const 
{ 
  return 
    s << "INTES(" 
      << Gaudi::Utils::toString ( location() ) << "," 
      << Gaudi::Utils::toString ( fullpath() ) << "]" ;
} 
// ============================================================================
LoKi::Particles::Identifier*
LoKi::Particles::Identifier::clone() const 
{ return new LoKi::Particles::Identifier(*this) ; }
// ============================================================================
LoKi::Particles::Identifier::result_type
LoKi::Particles::Identifier::operator() 
  ( LoKi::Particles::Identifier::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( " Invalid Particle, return 'InvalidID'" ) ;
    return LoKi::Constants::InvalidID ;
  } ;
  return p->particleID().pid() ;
} 
// ============================================================================
std::ostream& 
LoKi::Particles::Identifier::fillStream ( std::ostream& s ) const 
{ return s << "ID" ; } ;
// ============================================================================
LoKi::Particles::AbsIdentifier*
LoKi::Particles::AbsIdentifier::clone() const 
{ return new LoKi::Particles::AbsIdentifier(*this) ; }
// ============================================================================
LoKi::Particles::AbsIdentifier::result_type
LoKi::Particles::AbsIdentifier::operator() 
  ( LoKi::Particles::AbsIdentifier::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( " Invalid Particle, return 'InvalidID'" ) ;
    return LoKi::Constants::InvalidID ;
  } ;
  return p->particleID().abspid() ;
} 
// ============================================================================
std::ostream& 
LoKi::Particles::AbsIdentifier::fillStream ( std::ostream& s ) const 
{ return s << "ABSID" ; } ;
// ============================================================================
LoKi::Particles::Charge*
LoKi::Particles::Charge::clone() const 
{ return new LoKi::Particles::Charge(*this) ; }
// ============================================================================
LoKi::Particles::Charge::result_type
LoKi::Particles::Charge::operator() 
  ( LoKi::Particles::Charge::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( " Invalid Particle, return -1000" ) ;
    return -1000 ;
  } 
  return charge ( p );
} 
// ============================================================================
std::ostream& 
LoKi::Particles::Charge::fillStream ( std::ostream& s ) const 
{ return s << "Q" ; } ;
// ============================================================================
LoKi::Particles::SumCharge*
LoKi::Particles::SumCharge::clone() const 
{ return new LoKi::Particles::SumCharge(*this) ; }
// ============================================================================
LoKi::Particles::SumCharge::result_type
LoKi::Particles::SumCharge::sumCharge 
( LoKi::Particles::Charge::argument p ) const 
{
  if ( 0 == p ) 
  { Error ( " Invalid Particle, return -1000" ) ; return -1000 ; } ; // RETURN 
  // basic ? 
  if ( p->isBasicParticle() ) { return charge ( p ) ; }              // RETURN 
  // 
  typedef SmartRefVector<LHCb::Particle> DP;
  const DP& daughters = p->daughters() ;
  // no daughters ?
  if ( daughters.empty() ) 
  {
    Warning ( "Empty list of daughters for '" + 
              LoKi::Particles::nameFromPID ( p->particleID() ) + "'" ) ;
    return charge ( p )  ;                                         // RETURN 
  }
  double q = 0 ;
  for ( DP::const_iterator idau = daughters.begin() ; 
        daughters.end() != idau ; ++idau ) 
  { q += sumCharge ( *idau ) ; }                                 // RECURSION 
  // 
  return q ;
} 
// ============================================================================
std::ostream& 
LoKi::Particles::SumCharge::fillStream ( std::ostream& s ) const 
{ return s << "SUMQ" ; } ;
// ============================================================================
LoKi::Particles::Momentum* 
LoKi::Particles::Momentum::clone() const 
{ return new Momentum( *this ) ; }
// ============================================================================
LoKi::Particles::Momentum::result_type 
LoKi::Particles::Momentum::operator() 
  ( LoKi::Particles::Momentum::argument p ) const 
{
  if( 0 != p ) { return p -> p () ; }                        // RETURN 
  Error ( " Invalid Particle, return 'InvalidMomentum'" ) ;
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
std::ostream& LoKi::Particles::Momentum::fillStream( std::ostream& s ) const 
{ return s << "P" ; }
// ============================================================================
LoKi::Particles::Momentum2* 
LoKi::Particles::Momentum2::clone() const 
{ return new Momentum2( *this ) ; }
// ============================================================================
LoKi::Particles::Momentum2::result_type 
LoKi::Particles::Momentum2::operator() 
  ( LoKi::Particles::Momentum2::argument p ) const 
{
  if( 0 != p ) 
  { return p -> momentum().P2() ; }                 // RETURN 
  Error ( " Invalid Particle, return 'InvalidMomentum'" ) ;
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
std::ostream& 
LoKi::Particles::Momentum2::fillStream( std::ostream& s ) const 
{ return s << "P2" ; }
// ============================================================================
LoKi::Particles::Energy* 
LoKi::Particles::Energy::clone() const 
{ return new Energy( *this ) ; }
// ============================================================================
LoKi::Particles::Energy::result_type 
LoKi::Particles::Energy::operator() 
  ( LoKi::Particles::Energy::argument p ) const 
{
  if( 0 != p ) { return p -> momentum () .E ()  ; }          // RETURN 
  Error(" Invalid Particle, return 'InvalidEnergy'");
  return LoKi::Constants::InvalidEnergy;                     // RETURN 
}
// ============================================================================
std::ostream& 
LoKi::Particles::Energy::fillStream( std::ostream& s ) const 
{ return s << "E" ; }
// ============================================================================
LoKi::Particles::TransverseMomentum* 
LoKi::Particles::TransverseMomentum::clone() const 
{ return new TransverseMomentum( *this ) ; }
// ============================================================================
LoKi::Particles::TransverseMomentum::result_type 
LoKi::Particles::TransverseMomentum::operator() 
  ( LoKi::Particles::TransverseMomentum::argument p ) const 
{
  if ( 0 != p ) { return p -> momentum().Pt() ; }            // RETURN 
  Error(" Invalid Particle, return 'InvalidMomentum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
std::ostream& 
LoKi::Particles::TransverseMomentum::fillStream( std::ostream& s ) const 
{ return s << "PT" ; }
// ============================================================================
LoKi::Particles::MomentumX* 
LoKi::Particles::MomentumX::clone() const 
{ return new MomentumX( *this ) ; }
// ============================================================================
LoKi::Particles::MomentumX::result_type 
LoKi::Particles::MomentumX::operator() 
  ( LoKi::Particles::MomentumX::argument p ) const 
{
  if ( 0 != p ) { return p -> momentum () . Px () ; }         // RETURN 
  Error(" Invalid Particle, return 'InvalidMomentum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
std::ostream& 
LoKi::Particles::MomentumX::fillStream( std::ostream& s ) const 
{ return s << "PX" ; }
// ============================================================================
LoKi::Particles::MomentumY* 
LoKi::Particles::MomentumY::clone() const 
{ return new MomentumY( *this ) ; }
// ============================================================================
LoKi::Particles::MomentumY::result_type 
LoKi::Particles::MomentumY::operator() 
  ( LoKi::Particles::MomentumY::argument p ) const 
{
  if( 0 != p ) { return p -> momentum () . py () ; }         // RETURN 
  Error(" Invalid Particle, return 'InvalidMomentum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
std::ostream& 
LoKi::Particles::MomentumY::fillStream( std::ostream& s ) const 
{ return s << "PY" ; }
// ============================================================================
LoKi::Particles::MomentumZ* 
LoKi::Particles::MomentumZ::clone() const 
{ return new MomentumZ( *this ) ; }
// ============================================================================
LoKi::Particles::MomentumZ::result_type 
LoKi::Particles::MomentumZ::operator() 
  ( LoKi::Particles::MomentumZ::argument p ) const 
{
  if ( 0 != p ) { return p -> momentum () . Pz () ; }         // RETURN 
  Error(" Invalid Particle, return 'InvalidMomentum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
std::ostream& 
LoKi::Particles::MomentumZ::fillStream( std::ostream& s ) const 
{ return s << "PZ" ; }
// ============================================================================
LoKi::Particles::PseudoRapidity*
LoKi::Particles::PseudoRapidity::clone() const 
{ return new LoKi::Particles::PseudoRapidity ( *this ) ; }
// ============================================================================
LoKi::Particles::PseudoRapidity::result_type 
LoKi::Particles::PseudoRapidity::operator() 
  ( LoKi::Particles::PseudoRapidity::argument p ) const
{
  if( 0 != p ) { return eta ( p ) ; }                        // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
double LoKi::Particles::PseudoRapidity::y 
( const Gaudi::LorentzVector& v ) const
{
  const double e  = v.E  () ;
  const double pz = v.Pz () ;
  return 0.5*std::log( (e+pz)/(e-pz) ) ;
}
// ============================================================================
double LoKi::Particles::PseudoRapidity::y0
( const Gaudi::LorentzVector& v ) const
{
  const double e  = v.E  () ;
  const double p  = v.P  () ;
  return 0.5*std::log( (e+p)/(e-p) ) ;
}
// ============================================================================
std::ostream& 
LoKi::Particles::PseudoRapidity::fillStream( std::ostream& s ) const 
{ return s << "ETA" ; }
// ============================================================================
std::ostream& 
LoKi::Particles::Rapidity::fillStream( std::ostream& s ) const 
{ return s << "Y" ; }
// ============================================================================
std::ostream& 
LoKi::Particles::Rapidity0::fillStream( std::ostream& s ) const 
{ return s << "Y0" ; }
// ============================================================================


// ============================================================================
LoKi::Particles::Rapidity*
LoKi::Particles::Rapidity::clone() const 
{ return new LoKi::Particles::Rapidity ( *this ) ; }
// ============================================================================
LoKi::Particles::Rapidity::result_type 
LoKi::Particles::Rapidity::operator() 
  ( LoKi::Particles::Rapidity::argument p ) const
{
  if( 0 != p ) { return y ( p ) ; }                        // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
LoKi::Particles::Rapidity0*
LoKi::Particles::Rapidity0::clone() const 
{ return new LoKi::Particles::Rapidity0 ( *this ) ; }
// ============================================================================
LoKi::Particles::Rapidity0::result_type 
LoKi::Particles::Rapidity0::operator() 
  ( LoKi::Particles::Rapidity0::argument p ) const
{
  if ( 0 != p ) { return y0 ( p ) ; }                        // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================


// ============================================================================
LoKi::Particles::Phi::result_type 
LoKi::Particles::Phi::operator() 
  ( LoKi::Particles::Phi::argument p ) const
{
  if( 0 != p ) { return phi ( p ) ; }    // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
LoKi::Particles::Theta::result_type 
LoKi::Particles::Theta::operator() 
  ( LoKi::Particles::Theta::argument p ) const
{
  if( 0 != p ) { return p -> momentum () . Theta () ; }    // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
}
// ============================================================================
LoKi::Particles::Theta*
LoKi::Particles::Theta::clone() const 
{ return new LoKi::Particles::Theta(*this) ; }
// ============================================================================
std::ostream& 
LoKi::Particles::Theta::fillStream( std::ostream& s ) const 
{ return s << "THETA" ; }
// ============================================================================
LoKi::Particles::Mass::result_type 
LoKi::Particles::Mass::operator() 
  ( LoKi::Particles::Mass::argument p ) const 
{
  if ( 0 != p ) { return mass ( p )  ; }     // RETURN 
  Error(" Invalid Particle, return 'InvalidMass'");
  return LoKi::Constants::InvalidMass;                   // RETURN 
}
// ============================================================================
LoKi::Particles::MeasuredMass* 
LoKi::Particles::MeasuredMass::clone() const 
{ return new MeasuredMass( *this ) ; }
// ============================================================================
LoKi::Particles::MeasuredMass::result_type 
LoKi::Particles::MeasuredMass::operator() 
  ( LoKi::Particles::MeasuredMass::argument p ) const 
{
  if( 0 != p ) { return measuredMass  ( p ) ; }         // RETURN 
  Error(" Invalid Particle, return 'InvalidMass'");
  return LoKi::Constants::InvalidMass;                   // RETURN 
}
// ============================================================================
std::ostream& 
LoKi::Particles::MeasuredMass::fillStream( std::ostream& s ) const 
{ return s << "MM" ; }
// ============================================================================
/*  constructor with index of daughter particle 
 *  @attention <c>index==0</c> corresponds to the particle 
 */
LoKi::Particles::InvariantMass::InvariantMass
( const size_t index ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , m_indices() 
{ m_indices.push_back ( index ) ; }
// ============================================================================
/*  constructor with pair of indices of daughter particle 
 *  @attention <c>index==0</c> corresponds to the particle 
 */
// ============================================================================
LoKi::Particles::InvariantMass::InvariantMass
( const size_t index1    , 
  const size_t index2    ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , m_indices() 
{ 
  m_indices.push_back ( index1 ) ; 
  m_indices.push_back ( index2 ) ; 
  std::sort ( m_indices.begin() , m_indices.end () ) ;
}
// ============================================================================
/*  constructor with triplet of indices of daughter particle 
 *  @attention <c>index==0</c> corresponds to the particle 
 */
// ============================================================================
LoKi::Particles::InvariantMass::InvariantMass
( const size_t index1    , 
  const size_t index2    , 
  const size_t index3    ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , m_indices() 
{ 
  m_indices.push_back ( index1 ) ; 
  m_indices.push_back ( index2 ) ; 
  m_indices.push_back ( index3 ) ; 
  std::sort ( m_indices.begin() , m_indices.end () ) ;
}
// ============================================================================
/*  constructor with arbitrary number of indices of daughter particle 
 *  @attention <c>index==0</c> corresponds to the particle 
 */
// ============================================================================
LoKi::Particles::InvariantMass::InvariantMass
( const LoKi::Particles::InvariantMass::Indices& indices ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , m_indices( indices ) 
{
  std::sort ( m_indices.begin() , m_indices.end () ) ;
}
// ============================================================================
LoKi::Particles::InvariantMass* 
LoKi::Particles::InvariantMass::clone() const 
{ return new InvariantMass( *this ) ;}
// ============================================================================
/*  the only one essential method 
 *  @param  mother mother particle 
 *  @return the invariant mass for selected 
 *          combination of daughter particles
 */
// ============================================================================
double LoKi::Particles::InvariantMass::operator() 
  ( LoKi::Particles::InvariantMass::argument mother ) const 
{
  if ( 0 == mother   ) 
  { 
    Error(" 'Mother' is invalid, return 'InvalidMass' ");
    return LoKi::Constants::InvalidMass;                         // RETURN 
  }
  // the special case 
  if ( m_indices.empty() ) { return mother->momentum().M(); }  // RETURN 
  // the simplest case 
  if ( 1 == m_indices.size() && 0 == m_indices[0] ) 
  { return mother->momentum().M(); }                            // RETURN
  // collect daughters 
  LoKi::LorentzVector mom ;
  for( Indices::const_iterator index = m_indices.begin() ; 
       m_indices.end() != index ; ++index ) 
  {
    if ( 0 == *index   ) { mom += mother->momentum() ; continue ; } 
    const LHCb::Particle* daughter = 
      LoKi::Child::child( mother , *index ) ;
    if ( 0 == daughter ) 
    { 
      Error (" 'Daughter' is invalid, return 'InvalidMass'");
      return LoKi::Constants::InvalidMass;                     // RETURN
    }               
    mom += daughter->momentum();
  }
  //  
  return mom.M();
}
// ============================================================================
//  specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::InvariantMass::fillStream( std::ostream& s ) const 
{
  s << "MASS" ;
  s << "("    ;    
  for ( Indices::const_iterator it =  m_indices.begin() ; 
        m_indices.end() != it ; ++it ) 
  {
    if ( m_indices.begin() != it ) { s << "," ; } 
    s << (*it ) ;
  }
  s << ")" ;
  return s ;
}
// ============================================================================
/*  constructor 
 *  @param pp particle property 
 */
// ============================================================================
LoKi::Particles::DeltaMass::DeltaMass
( const LHCb::ParticleProperty&     pp     )
  : LoKi::Particles::Mass ()
  , m_mass ( pp.mass() ) 
{}
// ============================================================================
/*  constructor 
 *  @param name particle name 
 *  @param svp  ParticleProperty service 
 */
// ============================================================================
LoKi::Particles::DeltaMass::DeltaMass
( const std::string&          name   , 
  LHCb::IParticlePropertySvc* ppsvc  )
  : LoKi::Particles::Mass ()
  , m_mass ( 0 * Gaudi::Units::GeV ) 
{
  const LHCb::ParticleProperty* pp = 0 ; 
  if ( 0 != ppsvc ) { pp = ppsvc->find ( name ) ; }
  else              { pp = LoKi::Particles::ppFromName ( name ) ; }
  if ( 0 == pp    ) 
  { Exception ("DeltaMass(): Unknow particle name '"+name+"'");}
  m_mass = pp->mass();
}
// ============================================================================
/*  constructor 
 *  @param pid  particle ID 
 *  @param svp  ParticleProperty service 
 */
// ============================================================================
LoKi::Particles::DeltaMass::DeltaMass
( const LHCb::ParticleID&     pid    , 
  LHCb::IParticlePropertySvc* ppsvc  )
  : LoKi::Particles::Mass ()
  , m_mass ( 0 * Gaudi::Units::GeV )  
{
  const LHCb::ParticleProperty* pp = 0 ; 
  if ( 0 != ppsvc ) { pp = ppsvc->find                ( pid ) ; }
  else              { pp = LoKi::Particles::ppFromPID ( pid ) ; }
  if ( 0 == pp    ) 
  { Exception ( "DeltaMass(): Unknow ParticleID " ) ; }
  m_mass = pp->mass();  
}
// ============================================================================
LoKi::Particles::DeltaMass::result_type 
LoKi::Particles::DeltaMass::operator() 
  ( LoKi::Particles::DeltaMass::argument p ) const
{
  if ( 0 != p ) { return ( mass ( p ) - m_mass ) ; }    // RETURN 
  Error (" Invalid Particle, return 'InvalidMass'");
  return LoKi::Constants::InvalidMass;                   // RETURN 
}
// ============================================================================
// clone 
// ============================================================================
LoKi::Particles::DeltaMass*
LoKi::Particles::DeltaMass::clone() const 
{ return new DeltaMass ( *this ) ; }
// ============================================================================
//  specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::DeltaMass::fillStream( std::ostream& s ) const 
{ return s << "DMASS(" << m_mass << ")" ; }
// ============================================================================
// specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::AbsDeltaMass::fillStream( std::ostream& s ) const
{ return s << "ADMASS(" << m0() << ")" ; }
// ============================================================================
LoKi::Particles::AbsDeltaMass::result_type 
LoKi::Particles::AbsDeltaMass::operator() 
  ( LoKi::Particles::AbsDeltaMass::argument p ) const
{
  if ( 0 != p ) { return ::fabs ( mass ( p ) - m0 () ) ; }    // RETURN 
  Error (" Invalid Particle, return 'InvalidMass'");
  return LoKi::Constants::InvalidMass;                   // RETURN 
}
// ============================================================================
LoKi::Particles::DeltaMeasuredMass::result_type 
LoKi::Particles::DeltaMeasuredMass::operator() 
  ( LoKi::Particles::DeltaMeasuredMass::argument p ) const 
{
  if ( 0 != p ) { return ( measuredMass ( p ) - m0 ()  ) ; }        // RETURN 
  Error ( " Invalid Particle, return 'InvalidMass'");
  return LoKi::Constants::InvalidMass;                   // RETURN 
}
// ============================================================================
//  virtual destructor 
// ============================================================================
LoKi::Particles::DeltaMeasuredMass::~DeltaMeasuredMass(){}
// ============================================================================
//  clone method (mandatory!)
// ============================================================================
LoKi::Particles::DeltaMeasuredMass*
LoKi::Particles::DeltaMeasuredMass::clone() const 
{ return new DeltaMeasuredMass( *this ) ; }
// ============================================================================
//  specific printout
// ============================================================================
std::ostream& 
LoKi::Particles::DeltaMeasuredMass::fillStream( std::ostream& s ) const 
{ return s << "DMMASS("  << m0 ()  << ")"; }
// ============================================================================
//  destructor (virtual)
// ============================================================================
LoKi::Particles::AbsDeltaMeasuredMass::~AbsDeltaMeasuredMass(){}
// ============================================================================
//  MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::AbsDeltaMeasuredMass*
LoKi::Particles::AbsDeltaMeasuredMass::clone() const
{ return new LoKi::Particles::AbsDeltaMeasuredMass( *this ) ; }
// ============================================================================
//  MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::AbsDeltaMeasuredMass::result_type 
LoKi::Particles::AbsDeltaMeasuredMass::operator() 
  ( LoKi::Particles::AbsDeltaMeasuredMass::argument p ) const 
{
  if ( 0 != p ) { return ::fabs( measuredMass ( p ) - m0 () ) ; } // RETURN 
  Error(" Invalid Particle, return 'InvalidMass'");
  return LoKi::Constants::InvalidMass;                      // RETURN 
};
// ============================================================================
//  specific printout
// ============================================================================
std::ostream& 
LoKi::Particles::AbsDeltaMeasuredMass::fillStream( std::ostream& s ) const 
{ return s << "ADMMASS(" << m0 () << ")" ; }
// ============================================================================
LoKi::Particles::DeltaMeasuredMassChi2::result_type 
LoKi::Particles::DeltaMeasuredMassChi2::operator() 
  ( LoKi::Particles::DeltaMeasuredMassChi2::argument p ) const 
{
  if ( 0 == p              )
  {
    Error(" Invalid Particle, return 'InvalidMass'");
    return LoKi::Constants::InvalidMass ;                       // RETURN
  }
  if( 0 == p -> measuredMassErr() ) 
  {
    Error(" Invalid MassError, return 'InvalidMass'");
    return LoKi::Constants::InvalidMass ;                       // RETURN
  }
  //
  const double value = ( measuredMass ( p ) - m0() ) / p->measuredMassErr() ;
  //
  return value * value ;
}
// ============================================================================
LoKi::Particles::DeltaMeasuredMassChi2*
LoKi::Particles::DeltaMeasuredMassChi2::clone() const 
 { return new DeltaMeasuredMassChi2( *this ) ; }
// ============================================================================
// specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::DeltaMeasuredMassChi2::fillStream( std::ostream& s ) const 
{ return s << "CHI2MM("  << m0() <<  ")" ; }
// ============================================================================
LoKi::Particles::DeltaMassChi2::result_type 
LoKi::Particles::DeltaMassChi2::operator() 
  ( LoKi::Particles::DeltaMassChi2::argument p ) const 
{
  if ( 0 == p              )
  {
    Error(" Invalid Particle, return 'InvalidChi2'");
    return LoKi::Constants::InvalidChi2 ;                       // RETURN
  }
  return LoKi::Kinematics::chi2mass 
    ( m0() , p->momentum() , p->momCovMatrix() ) ;                                   
}
// ============================================================================
// specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::DeltaMassChi2::fillStream( std::ostream& s ) const 
{ return s << "CHI2M("  << m0() <<  ")" ; }
// ============================================================================
LoKi::Particles::ConfidenceLevel* 
LoKi::Particles::ConfidenceLevel::clone() const 
{ return new ConfidenceLevel( *this ) ; }
// ============================================================================
LoKi::Particles::ConfidenceLevel::result_type 
LoKi::Particles::ConfidenceLevel::operator() 
  ( LoKi::Particles::ConfidenceLevel::argument p ) const 
{
  if ( 0 != p ) { return p -> confLevel () ; }              // RETURN 
  Error ( " Invalid Particle, return '-1000'" ) ;
  return -1000 ;                                           // RETURN 
}
// ============================================================================
std::ostream& 
LoKi::Particles::ConfidenceLevel::fillStream
( std::ostream& s ) const 
{ return s << "CL" ; }
// ============================================================================
LoKi::Particles::Weight* 
LoKi::Particles::Weight::clone() const 
{ return new Weight( *this ) ; }
// ============================================================================
LoKi::Particles::Weight::result_type 
LoKi::Particles::Weight::operator() 
  ( LoKi::Particles::Weight::argument p ) const 
{
  if ( 0 != p ) { return p -> weight () ; }              // RETURN 
  Error ( " Invalid Particle, return '-1000'" ) ;
  return -1000 ;                                           // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Particles::Weight::fillStream
( std::ostream& s ) const 
{ return s << "WEIGHT" ; }
// ============================================================================
LoKi::Particles::IsBasic* 
LoKi::Particles::IsBasic::clone() const 
{ return new IsBasic( *this ) ; }
// ============================================================================
LoKi::Particles::IsBasic::result_type 
LoKi::Particles::IsBasic::operator() 
  ( LoKi::Particles::IsBasic::argument p ) const 
{
  if ( 0 != p ) { return p -> isBasicParticle () ; }      // RETURN 
  Error ( " Invalid Particle, return 'false'" ) ;
  return false;                                           // RETURN 
}
// ============================================================================
std::ostream& 
LoKi::Particles::IsBasic::fillStream
( std::ostream& s ) const 
{ return s << "BASIC" ; }
// ============================================================================
LoKi::Particles::NumberOfDaughters* 
LoKi::Particles::NumberOfDaughters::clone() const 
{ return new NumberOfDaughters( *this ) ; }
// ============================================================================
LoKi::Particles::NumberOfDaughters::result_type 
LoKi::Particles::NumberOfDaughters::operator() 
  ( LoKi::Particles::NumberOfDaughters::argument p ) const 
{
  if ( 0 != p ) { return p -> daughters().size() ; }   // RETURN 
  Error ( " Invalid Particle, return 0" ) ;
  return 0 ;                                           // RETURN 
}
// ============================================================================
std::ostream& 
LoKi::Particles::NumberOfDaughters::fillStream
( std::ostream& s ) const 
{ return s << "NDAUGS" ; }
// ============================================================================
LoKi::Particles::HasProto* 
LoKi::Particles::HasProto::clone() const 
{ return new HasProto( *this ) ; }
// ============================================================================
LoKi::Particles::HasProto::result_type 
LoKi::Particles::HasProto::operator() 
  ( LoKi::Particles::HasProto::argument p ) const 
{
  if ( 0 != p ) { return ( 0 != p ->proto() ) ; }         // RETURN 
  Error ( " Invalid Particle, return false" ) ;
  return false ;                                           // RETURN 
}
// ============================================================================
std::ostream& 
LoKi::Particles::HasProto::fillStream
( std::ostream& s ) const 
{ return s << "HASPROTO" ; }
// ============================================================================
LoKi::Particles::HasVertex* 
LoKi::Particles::HasVertex::clone() const 
{ return new HasVertex( *this ) ; }
// ============================================================================
LoKi::Particles::HasVertex::result_type 
LoKi::Particles::HasVertex::operator() 
  ( LoKi::Particles::HasVertex::argument p ) const 
{
  if ( 0 != p ) { return ( 0 != p ->endVertex() ) ; }     // RETURN 
  Error ( " Invalid Particle, return false" ) ;
  return false ;                                           // RETURN 
}
// ============================================================================
std::ostream& 
LoKi::Particles::HasVertex::fillStream
( std::ostream& s ) const 
{ return s << "HASVERTEX" ; }
// ============================================================================
LoKi::Particles::HasTrack::result_type 
LoKi::Particles::HasTrack::operator() 
  ( LoKi::Particles::HasTrack::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( " Invalid Particle, return false" ) ;
    return false ;                                                // RETURN
  }
  // get protoparticle 
  const LHCb::ProtoParticle* pp = p -> proto() ;
  if ( 0 == pp ) { return false ; }                               // RETURN
  // check validity of the track:
  return 0 != pp->track() ;
}
// ============================================================================
std::ostream& 
LoKi::Particles::HasTrack::fillStream
( std::ostream& s ) const 
{ return s << "HASTRACK" ; }
// ============================================================================
// constructor from theta phi
// ============================================================================
LoKi::Particles::TransverseMomentumRel::TransverseMomentumRel
( const double theta , 
  const double phi   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_momentum ( ) 
{
  m_momentum.SetXYZ 
    ( ::sin ( theta ) * ::cos ( phi ) ,
      ::sin ( theta ) * ::sin ( phi ) ,
      ::cos ( theta )                 ) ;
}
// ============================================================================
//  constructor from 3-vector 
// ============================================================================
LoKi::Particles::TransverseMomentumRel::TransverseMomentumRel
( const LoKi::ThreeVector&  v ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_momentum ( v ) 
{} 
// ============================================================================
//  constructor from 4-vector 
// ============================================================================
LoKi::Particles::TransverseMomentumRel::TransverseMomentumRel
( const LoKi::LorentzVector&  v ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_momentum ( v ) 
{} 
// ============================================================================
//  constructor from LHCb::Particle
// ============================================================================
LoKi::Particles::TransverseMomentumRel::TransverseMomentumRel
( const LHCb::Particle*  v ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_momentum () 
{
  if ( 0 == v ) { Exception ( "Invalid LHCb::Particle*!" ) ; }
  m_momentum = v->momentum() ;
} 
// ============================================================================
//  copy constructor 
// ============================================================================
LoKi::Particles::TransverseMomentumRel::TransverseMomentumRel 
( const LoKi::Particles::TransverseMomentumRel& right ) 
  : LoKi::AuxFunBase                     ( right ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function( right ) 
  , m_momentum ( right.m_momentum ) 
{} 
// ============================================================================
//  MANDATORY: clone method ("virtual constructor") 
// ============================================================================
LoKi::Particles::TransverseMomentumRel*
LoKi::Particles::TransverseMomentumRel::clone() const 
{ return new LoKi::Particles::TransverseMomentumRel(*this) ; }
// ============================================================================
//  MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::TransverseMomentumRel::~TransverseMomentumRel(){}
// ============================================================================
//  MANDATORY: the only one essential mehtod 
// ============================================================================
LoKi::Particles::TransverseMomentumRel::result_type 
LoKi::Particles::TransverseMomentumRel::operator() 
  ( LoKi::Particles::TransverseMomentumRel::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("Invalid argument, return 'Invalid Momentum'") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  // use the function
  return LoKi::Kinematics::transverseMomentumDir( p->momentum() , 
                                                  m_momentum );
} 
// ============================================================================
//  OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::TransverseMomentumRel::fillStream( std::ostream& s ) const 
{ return s << "PTDIR(" 
           << m_momentum.Theta () << ","
           << m_momentum.Phi   () <<  ")" ; }
// ============================================================================



// ============================================================================
// constructor from the daughter's index 
// ============================================================================
LoKi::Particles::TransverseMomentumQ::TransverseMomentumQ
( const unsigned int index ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()  
  , m_index ( index ) 
{}
// ============================================================================
//  MANDATORY: the only one essential mehtod 
// ============================================================================
LoKi::Particles::TransverseMomentumQ::result_type 
LoKi::Particles::TransverseMomentumQ::operator() 
  ( LoKi::Particles::TransverseMomentumQ::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("Invalid argument, return 'Invalid Momentum'") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  // get the proper daughter 
  const LHCb::Particle* d = LoKi::Child::child ( p , m_index ) ; 
  //
  if ( 0 == d ) 
  {
    Error( "Invalid daughter, return 'Invalid Momentum'") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  // use the function 
  return LoKi::Kinematics::transverseMomentumDir 
    ( d -> momentum() , p -> momentum () . Vect()  ) ;
}
// ============================================================================
//  OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::TransverseMomentumQ::fillStream( std::ostream& s ) const 
{ 
  if      ( 1 == m_index ) { return s  << "QPT1" ; }                 // RETURN
  else if ( 2 == m_index ) { return s  << "QPT2" ; }                 // RETURN
  //
  return s << "QPT(" << m_index << ")" ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================ 
LoKi::Particles::NominalMass::result_type 
LoKi::Particles::NominalMass::operator() 
  ( LoKi::Particles::NominalMass::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("Invalid argument, return 'Invalid Mass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  const LHCb::ParticleProperty* pp =
    LoKi::Particles::_ppFromPID ( p -> particleID() ) ;
  //
  if ( 0 == pp ) 
  {
    Error("Invalid particleID , return 'Invalid Mass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  return pp->mass() ;  
}


// ============================================================================
// The END 
// ============================================================================

