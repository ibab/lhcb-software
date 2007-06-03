// $Id: Particles0.cpp,v 1.11 2007-06-03 20:41:36 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.11 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.10  2007/06/01 12:07:03  ibelyaev
//  prepare for version v4r0
//
// Revision 1.9  2007/04/16 16:16:27  pkoppenb
// removed polemic comment
//
// Revision 1.8  2007/03/01 16:14:40  ibelyaev
//  add PTREL and PTDIR functions
//
//
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ToStream.h"
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
  : LoKi::Function<const LHCb::Particle*> ()
  , m_bad   ( bad ) 
  , m_nokey ( bad ) 
{} 
// ============================================================================
LoKi::Particles::Key::Key
( const LHCb::Particle::key_type bad   ,
  const LHCb::Particle::key_type nokey ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_bad   ( bad   ) 
  , m_nokey ( nokey ) 
{} 
// ============================================================================
LoKi::Particles::Key::Key
( const LoKi::Particles::Key& right ) 
  : LoKi::AuxFunBase                      ( right ) 
  , LoKi::Function<const LHCb::Particle*> ( right ) 
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
  : LoKi::Predicate<const LHCb::Particle*> ()
  , m_location ( path ) 
  , m_fullpath ( full ) 
{} 
// ============================================================================
LoKi::Particles::InTES::InTES 
( const LoKi::Particles::InTES& right ) 
  : LoKi::AuxFunBase                       ( right ) 
  , LoKi::Predicate<const LHCb::Particle*> ( right )
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
} ;
// ============================================================================
std::ostream& 
LoKi::Particles::Identifier::fillStream ( std::ostream& s ) const 
{ return s << "ID" ; } ;
// ============================================================================

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
} ;
// ============================================================================
std::ostream& 
LoKi::Particles::AbsIdentifier::fillStream ( std::ostream& s ) const 
{ return s << "ABSID" ; } ;
// ============================================================================


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
  } ;
  return p->charge();
} ;
// ============================================================================
std::ostream& 
LoKi::Particles::Charge::fillStream ( std::ostream& s ) const 
{ return s << "Q" ; } ;
// ============================================================================

// ============================================================================
LoKi::Particles::SumCharge*
LoKi::Particles::SumCharge::clone() const 
{ return new LoKi::Particles::SumCharge(*this) ; }
// ============================================================================
LoKi::Particles::SumCharge::result_type
LoKi::Particles::SumCharge::operator() 
  ( LoKi::Particles::Charge::argument p ) const 
{ return _charge( p ) ;}
// ============================================================================
LoKi::Particles::SumCharge::result_type
LoKi::Particles::SumCharge::_charge 
( LoKi::Particles::Charge::argument p ) const 
{
  if ( 0 == p ) 
  { Error ( " Invalid Particle, return -1000" ) ; return -1000 ; } ; // RETURN 
  if ( p->isBasicParticle() ) { return m_charge ( p ) ; }            // RETURN 
  // 
  typedef SmartRefVector<LHCb::Particle> DP;
  const DP& daughters = p->daughters() ;
  if ( daughters.empty() ) 
  {
    Warning ( "Empty list of daughters for '" + 
              LoKi::Particles::nameFromPID ( p->particleID() ) + "'" ) ;
    return m_charge ( p )  ;                                      // RETURN 
  }
  double charge = 0 ;
  for ( DP::const_iterator idau = daughters.begin() ; 
        daughters.end() != idau ; ++idau ) 
  { charge += _charge ( *idau ) ; }                               // RECURSION 
  ///
  return charge ;
} ;
// ============================================================================
std::ostream& 
LoKi::Particles::SumCharge::fillStream ( std::ostream& s ) const 
{ return s << "SUMQ" ; } ;
// ============================================================================

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
};
// ============================================================================
std::ostream& LoKi::Particles::Momentum::fillStream( std::ostream& s ) const 
{ return s << "P" ; }
// ============================================================================

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
};
// ============================================================================
std::ostream& 
LoKi::Particles::Momentum2::fillStream( std::ostream& s ) const 
{ return s << "P2" ; }
// ============================================================================

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
};
// ============================================================================
std::ostream& 
LoKi::Particles::Energy::fillStream( std::ostream& s ) const 
{ return s << "E" ; }
// ============================================================================


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
};
// ============================================================================
std::ostream& 
LoKi::Particles::TransverseMomentum::fillStream( std::ostream& s ) const 
{ return s << "PT" ; }
// ============================================================================

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
};
// ============================================================================
std::ostream& 
LoKi::Particles::MomentumX::fillStream( std::ostream& s ) const 
{ return s << "PX" ; }
// ============================================================================

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
};
// ============================================================================
std::ostream& 
LoKi::Particles::MomentumY::fillStream( std::ostream& s ) const 
{ return s << "PY" ; }
// ============================================================================

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
};
// ============================================================================
std::ostream& 
LoKi::Particles::MomentumZ::fillStream( std::ostream& s ) const 
{ return s << "PZ" ; }
// ============================================================================

// ============================================================================
LoKi::Particles::PseudoRapidity::result_type 
LoKi::Particles::PseudoRapidity::operator() 
  ( LoKi::Particles::PseudoRapidity::argument p ) const
{
  if( 0 != p ) { return p -> momentum () . Eta () ; }    // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
};
// ============================================================================
LoKi::Particles::PseudoRapidity*
LoKi::Particles::PseudoRapidity::clone() const 
{ return new LoKi::Particles::PseudoRapidity(*this) ; }
// ============================================================================
std::ostream& 
LoKi::Particles::PseudoRapidity::fillStream( std::ostream& s ) const 
{ return s << "ETA" ; }
// ============================================================================

// ============================================================================
LoKi::Particles::Phi::result_type 
LoKi::Particles::Phi::operator() 
  ( LoKi::Particles::Phi::argument p ) const
{
  if( 0 != p ) { return p -> momentum () . Phi () ; }    // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
};
// ============================================================================
LoKi::Particles::Phi*
LoKi::Particles::Phi::clone() const 
{ return new LoKi::Particles::Phi(*this) ; }
// ============================================================================
std::ostream& 
LoKi::Particles::Phi::fillStream( std::ostream& s ) const 
{ return s << "PHI" ; }
// ============================================================================

// ============================================================================
LoKi::Particles::Theta::result_type 
LoKi::Particles::Theta::operator() 
  ( LoKi::Particles::Theta::argument p ) const
{
  if( 0 != p ) { return p -> momentum () . Theta () ; }    // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
};
// ============================================================================
LoKi::Particles::Theta*
LoKi::Particles::Theta::clone() const 
{ return new LoKi::Particles::Theta(*this) ; }
// ============================================================================
std::ostream& 
LoKi::Particles::Theta::fillStream( std::ostream& s ) const 
{ return s << "THETA" ; }
// ============================================================================

// ============================================================================
LoKi::Particles::Mass* 
LoKi::Particles::Mass::clone() const 
{ return new Mass( *this ) ; }
// ============================================================================
LoKi::Particles::Mass::result_type 
LoKi::Particles::Mass::operator() 
  ( LoKi::Particles::Mass::argument p ) const 
{
  if ( 0 != p ) { return p -> momentum () . M  () ; }     // RETURN 
  Error(" Invalid Particle, return 'InvalidMass'");
  return LoKi::Constants::InvalidMass;                   // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Particles::Mass::fillStream( std::ostream& s ) const 
{ return s << "M" ; }
// ============================================================================

// ============================================================================
LoKi::Particles::MeasuredMass* 
LoKi::Particles::MeasuredMass::clone() const 
{ return new MeasuredMass( *this ) ; }
// ============================================================================
LoKi::Particles::MeasuredMass::result_type 
LoKi::Particles::MeasuredMass::operator() 
  ( LoKi::Particles::MeasuredMass::argument p ) const 
{
  if( 0 != p ) { return p -> measuredMass  () ; }             // RETURN 
  Error(" Invalid Particle, return 'InvalidMass'");
  return LoKi::Constants::InvalidMass;                   // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Particles::MeasuredMass::fillStream( std::ostream& s ) const 
{ return s << "MM" ; }
// ============================================================================

// ============================================================================
/** constructor with index of daughter particle 
 *  @attention <c>index==0</c> corresponds to the particle 
 */
LoKi::Particles::InvariantMass::InvariantMass
( const size_t index ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_indices() 
{ m_indices.push_back ( index ) ; }
// ============================================================================

// ============================================================================
/** constructor with pair of indices of daughter particle 
 *  @attention <c>index==0</c> corresponds to the particle 
 */
// ============================================================================
LoKi::Particles::InvariantMass::InvariantMass
( const size_t index1    , 
  const size_t index2    ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_indices() 
{ 
  m_indices.push_back ( index1 ) ; 
  m_indices.push_back ( index2 ) ; 
};
// ============================================================================

// ============================================================================
/** constructor with triplet of indices of daughter particle 
 *  @attention <c>index==0</c> corresponds to the particle 
 */
// ============================================================================
LoKi::Particles::InvariantMass::InvariantMass
( const size_t index1    , 
  const size_t index2    , 
  const size_t index3    ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_indices() 
{ 
  m_indices.push_back ( index1 ) ; 
  m_indices.push_back ( index2 ) ; 
  m_indices.push_back ( index3 ) ; 
};
// ============================================================================

// ============================================================================
/** constructor with arbitrary number of indices of daughter particle 
 *  @attention <c>index==0</c> corresponds to the particle 
 */
// ============================================================================
LoKi::Particles::InvariantMass::InvariantMass
( const LoKi::Particles::InvariantMass::Indices& indices ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_indices( indices ) 
{};
// ============================================================================
LoKi::Particles::InvariantMass* 
LoKi::Particles::InvariantMass::clone() const 
{ return new InvariantMass( *this ) ;}
// ============================================================================
/** the only one essential method 
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
  /// 
  return mom.M();
};
// ============================================================================
/// specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::InvariantMass::fillStream( std::ostream& s ) const 
{
  s << "M[" ;
  for( Indices::const_iterator it =  m_indices.begin() ; 
       m_indices.end() != it ; ++it ) 
  {
    if ( m_indices.begin() != it ) { s << "," ; } 
    s << (*it ) ;
  }
  s << "]" ;
  return s ;
}
// ============================================================================

// ============================================================================
/** constructor 
 *  @param pp particle property 
 */
// ============================================================================
LoKi::Particles::DeltaMass::DeltaMass
( const ParticleProperty&     pp     )
  : LoKi::Function<const LHCb::Particle*> ()
  , m_mass ( pp.mass() ) 
  , m_eval () 
{};
// ============================================================================
/** constructor 
 *  @param name particle name 
 *  @param svp  ParticleProperty service 
 */
// ============================================================================
LoKi::Particles::DeltaMass::DeltaMass
( const std::string&    name   , 
  IParticlePropertySvc* ppsvc  )
  : LoKi::Function<const LHCb::Particle*> ()
  , m_mass ( 0 * Gaudi::Units::GeV ) 
  , m_eval ()
{
  if ( 0 == ppsvc ) 
  { Exception("DeltaMass(): IParticlePropertySvc* is invalid!");}
  const ParticleProperty* pp = ppsvc->find( name );
  if ( 0 == pp    ) 
  { Exception("DeltaMass(): Unknow particle name '"+name+"'");}
  m_mass = pp->mass();
};
// ============================================================================
/** constructor 
 *  @param name particle name 
 */
// ============================================================================
LoKi::Particles::DeltaMass::DeltaMass
( const std::string&    name   )
  : LoKi::Function<const LHCb::Particle*> ()
  , m_mass ( 0 * Gaudi::Units::GeV )
  , m_eval ()
{
  const ParticleProperty* pp = LoKi::Particles::ppFromName(  name ) ;
  if ( 0 == pp    ) 
  { Exception ( "DeltaMass(): Unknow particle name '"+name+"'");}
  m_mass = pp->mass();
};
// ============================================================================
/** constructor 
 *  @param pid  particle ID 
 *  @param svp  ParticleProperty service 
 */
// ============================================================================
LoKi::Particles::DeltaMass::DeltaMass
( const LHCb::ParticleID&     pid    , 
  IParticlePropertySvc* ppsvc  )
  : LoKi::Function<const LHCb::Particle*> ()
  , m_mass ( 0 * Gaudi::Units::GeV )  
  , m_eval ()
{
  if ( 0 == ppsvc ) 
  { Exception("DeltaMass(): IParticlePropertySvc* is invalid!");}
  const ParticleProperty* pp = ppsvc->findByStdHepID( pid.pid() );
  if ( 0 == pp    ) 
  { Exception("DeltaMass(): Unknow ParticleID " ) ; }
  m_mass = pp->mass();  
};
// ============================================================================
/** constructor 
 *  @param pid  particle ID 
 *  @param svp  ParticleProperty service 
 */
// ============================================================================
LoKi::Particles::DeltaMass::DeltaMass
( const LHCb::ParticleID&     pid    )
  : LoKi::Function<const LHCb::Particle*> ()
  , m_mass ( 0 * Gaudi::Units::GeV )  
  , m_eval ()
{
  const ParticleProperty* pp = LoKi::Particles::ppFromPID( pid.pid() ) ;
  if( 0 == pp    ) 
  { Exception("DeltaMass(): Unknow ParticleID " ) ; }
  m_mass = pp->mass();  
};
// ============================================================================
LoKi::Particles::DeltaMass::DeltaMass
( const LoKi::Particles::DeltaMass& right )
  : LoKi::AuxFunBase                       ( right ) 
  , LoKi::Function<const LHCb::Particle*> ( right ) 
  , m_mass ( right.m_mass )
  , m_eval ( right.m_eval ) 
{}
// ============================================================================
LoKi::Particles::DeltaMass::result_type 
LoKi::Particles::DeltaMass::operator() 
  ( LoKi::Particles::DeltaMass::argument p ) const
{
  if ( 0 != p ) { return ( m_eval ( p ) - m_mass ) ; }    // RETURN 
  Error (" Invalid Particle, return 'InvalidMass'");
  return LoKi::Constants::InvalidMass;                   // RETURN 
};
// ============================================================================
// clone 
// ============================================================================
LoKi::Particles::DeltaMass*
LoKi::Particles::DeltaMass::clone() const 
{ return new DeltaMass ( *this ) ; }
// ============================================================================
/// specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::DeltaMass::fillStream( std::ostream& s ) const 
{ return s << "DM[" << m_mass << "]" ; }
// ============================================================================

// ============================================================================
/** constructor  
 *  @param mass nominal mass 
 */
// ============================================================================
LoKi::Particles::AbsDeltaMass::AbsDeltaMass( const double mass ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_eval ( mass ) {};
// ============================================================================
/** constructor 
 *  @param pp particle property 
 */
// ============================================================================
LoKi::Particles::AbsDeltaMass::AbsDeltaMass ( const ParticleProperty& pp ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_eval ( pp ) {}
// ============================================================================
/** constructor 
 *  @param name particle name 
 *  @param svp  ParticleProperty service 
 */
// ============================================================================
LoKi::Particles::AbsDeltaMass::AbsDeltaMass 
( const std::string&      name   , 
  IParticlePropertySvc*   ppsvc  ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_eval ( name , ppsvc ) {}
// ============================================================================
/** constructor 
 *  @param pid  particle ID 
 *  @param svp  ParticleProperty service 
 */
// ============================================================================
LoKi::Particles::AbsDeltaMass::AbsDeltaMass
( const LHCb::ParticleID& pid    , 
  IParticlePropertySvc*   ppsvc  ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_eval ( pid , ppsvc ) {}
// ============================================================================
/** constructor 
 *  @param name particle name 
 */
// ============================================================================
LoKi::Particles::AbsDeltaMass::AbsDeltaMass 
( const std::string&      name   ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_eval ( name  ) {}
// ============================================================================
/** constructor 
 *  @param pid  particle ID 
 */
// ============================================================================
LoKi::Particles::AbsDeltaMass::AbsDeltaMass 
( const LHCb::ParticleID&       pid    ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_eval ( pid ) {}
// ============================================================================
LoKi::Particles::AbsDeltaMass::AbsDeltaMass
( const LoKi::Particles::DeltaMass& right ) 
  : LoKi::Function<const LHCb::Particle*> () 
  , m_eval ( right ) {}
// ============================================================================
LoKi::Particles::AbsDeltaMass::AbsDeltaMass
( const LoKi::Particles::AbsDeltaMass& right ) 
  : LoKi::AuxFunBase                      ( right ) 
  , LoKi::Function<const LHCb::Particle*> ( right ) 
  , m_eval ( right.m_eval ) {}
// ============================================================================
LoKi::Particles::AbsDeltaMass::result_type 
LoKi::Particles::AbsDeltaMass::operator() 
  ( LoKi::Particles::AbsDeltaMass::argument p ) const
{ return std::fabs( m_eval( p ) ) ; }
// ============================================================================
// clone function 
// ============================================================================
LoKi::Particles::AbsDeltaMass*
LoKi::Particles::AbsDeltaMass::clone() const
{ return new AbsDeltaMass( *this ) ; }
// ============================================================================
// specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::AbsDeltaMass::fillStream( std::ostream& s ) const
{ return s << "|" << m_eval << "|" ; }
// ============================================================================

// ============================================================================
/// constructor  
// ============================================================================
LoKi::Particles::DeltaMeasuredMass::DeltaMeasuredMass 
( const double mass ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_mass ( mass ) , m_eval () {};
// ============================================================================
LoKi::Particles::DeltaMeasuredMass::result_type 
LoKi::Particles::DeltaMeasuredMass::operator() 
  ( LoKi::Particles::DeltaMeasuredMass::argument p ) const 
{
  if ( 0 != p ) { return ( m_eval ( p ) - m_mass ) ; }        // RETURN 
  Error ( " Invalid Particle, return 'InvalidMass'");
  return LoKi::Constants::InvalidMass;                   // RETURN 
};
// ============================================================================
/** constructor 
 *  @param pp particle property 
 */
// ============================================================================
LoKi::Particles::DeltaMeasuredMass::DeltaMeasuredMass
( const ParticleProperty&     pp     )
  : LoKi::Function<const LHCb::Particle*> ()
  , m_mass ( pp.mass() ) 
  , m_eval () 
{};
// ============================================================================
/** constructor 
 *  @param name particle name 
 *  @param svp  ParticleProperty service 
 */
// ============================================================================
LoKi::Particles::DeltaMeasuredMass::DeltaMeasuredMass
( const std::string&    name   , 
  IParticlePropertySvc* ppsvc  )
  : LoKi::Function<const LHCb::Particle*> ()
  , m_mass ( 0 * Gaudi::Units::GeV ) 
  , m_eval ()
{
  if ( 0 == ppsvc ) 
  { Exception("DeltaMass(): IParticlePropertySvc* is invalid!");}
  const ParticleProperty* pp = ppsvc->find( name );
  if ( 0 == pp    ) 
  { Exception("DeltaMass(): Unknow particle name '"+name+"'");}
  m_mass = pp->mass();
};
// ============================================================================
/** constructor 
 *  @param name particle name 
 *  @param svp  ParticleProperty service 
 */
// ============================================================================
LoKi::Particles::DeltaMeasuredMass::DeltaMeasuredMass
( const std::string&    name  )
  : LoKi::Function<const LHCb::Particle*> ()
  , m_mass ( 0 * Gaudi::Units::GeV ) 
  , m_eval ()
{
  IParticlePropertySvc* ppsvc = LoKi::Services::instance().ppSvc();
  if ( 0 == ppsvc ) 
  { Exception("DeltaMass(): IParticlePropertySvc* is invalid!");}
  const ParticleProperty* pp = ppsvc->find( name );
  if ( 0 == pp    ) 
  { Exception("DeltaMass(): Unknow particle name '"+name+"'");}
  m_mass = pp->mass();
};
// ============================================================================
/** constructor 
 *  @param pid  particle ID 
 *  @param svp  ParticleProperty service 
 */
// ============================================================================
LoKi::Particles::DeltaMeasuredMass::DeltaMeasuredMass
( const LHCb::ParticleID&  pid    , 
  IParticlePropertySvc*    ppsvc  )
  : LoKi::Function<const LHCb::Particle*> ()
  , m_mass ( 0 * Gaudi::Units::GeV )  
  , m_eval ()
{
  if( 0 == ppsvc ) 
  { Exception ("DeltaMeaqsuredMass(): IParticlePropertySvc* is invalid!");}
  const ParticleProperty* pp = ppsvc->findByStdHepID( pid.pid() );
  if( 0 == pp    ) 
  { Exception ("DeltaMeasuredMass(): Unknow ParticleID " ) ; }
  m_mass = pp->mass();  
};
// ============================================================================
/** constructor 
 *  @param pid  particle ID 
 *  @param svp  ParticleProperty service 
 */
// ============================================================================
LoKi::Particles::DeltaMeasuredMass::DeltaMeasuredMass
( const LHCb::ParticleID&     pid  )
  : LoKi::Function<const LHCb::Particle*> ()
  , m_mass ( 0 * Gaudi::Units::GeV )  
  , m_eval ()
{
  IParticlePropertySvc* ppsvc = LoKi::Services::instance().ppSvc();
  if( 0 == ppsvc ) 
  { Exception ( "DeltaMeasuredMass(): IParticlePropertySvc* is invalid!");}
  const ParticleProperty* pp = ppsvc->findByStdHepID( pid.pid() );
  if( 0 == pp    ) 
  { Exception ("DeltaMeasuredMass(): Unknow ParticleID " ) ; }
  m_mass = pp->mass();  
};
// ============================================================================
/** copy constructor 
 *  @param rigth object to be copied 
 */
// ============================================================================
LoKi::Particles::DeltaMeasuredMass::DeltaMeasuredMass
( const LoKi::Particles::DeltaMeasuredMass& right ) 
  : LoKi::AuxFunBase                      ( right ) 
  , LoKi::Function<const LHCb::Particle*> ( right )
  , m_mass ( right.m_mass ) 
  , m_eval ( right.m_eval ) {};
// ============================================================================
/// virtual destructor 
// ============================================================================
LoKi::Particles::DeltaMeasuredMass::~DeltaMeasuredMass(){}
// ============================================================================
/// clone method (mandatory!)
// ============================================================================
LoKi::Particles::DeltaMeasuredMass*
LoKi::Particles::DeltaMeasuredMass::clone() const 
{ return new DeltaMeasuredMass( *this ) ; };
// ============================================================================
/// specific printout
// ============================================================================
std::ostream& 
LoKi::Particles::DeltaMeasuredMass::fillStream( std::ostream& s ) const 
{ return s << "DMM["  << m_mass << "]"; };
// ============================================================================

// ============================================================================
/** constructor  from particle mass 
 *  @param mass particle mass 
 */
// ============================================================================
LoKi::Particles::AbsDeltaMeasuredMass::AbsDeltaMeasuredMass 
( const double mass ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_eval ( mass ) {};
// ============================================================================
/** constructor 
 *  @param pp particle property 
 */
// ============================================================================
LoKi::Particles::AbsDeltaMeasuredMass::AbsDeltaMeasuredMass 
( const ParticleProperty& pp ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_eval( pp ) {};
// ============================================================================
/** constructor 
 *  @param name particle name 
 *  @param svp  ParticleProperty service 
 */
// ============================================================================
LoKi::Particles::AbsDeltaMeasuredMass::AbsDeltaMeasuredMass
( const std::string&      name   , 
  IParticlePropertySvc*   ppsvc  )
  : LoKi::Function<const LHCb::Particle*> ()
  , m_eval ( name , ppsvc ) {} ;
// ============================================================================
/** constructor 
 *  @param pid  particle ID 
 *  @param svp  ParticleProperty service 
 */
// ============================================================================
LoKi::Particles::AbsDeltaMeasuredMass::AbsDeltaMeasuredMass
( const LHCb::ParticleID& pid    , 
  IParticlePropertySvc*   ppsvc  )
  : LoKi::Function<const LHCb::Particle*> ()
  , m_eval ( pid  , ppsvc ) {} ;
// ============================================================================
/** constructor 
 *  @param name particle name 
 */
// ============================================================================
LoKi::Particles::AbsDeltaMeasuredMass::AbsDeltaMeasuredMass
( const std::string&      name   ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_eval ( name ) {} ;
// ============================================================================
/** constructor 
 *  @param pid  particle ID 
 */
// ============================================================================
LoKi::Particles::AbsDeltaMeasuredMass::AbsDeltaMeasuredMass
( const LHCb::ParticleID&       pid    ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_eval ( pid ) {} ;
// ============================================================================/
LoKi::Particles::AbsDeltaMeasuredMass::AbsDeltaMeasuredMass
( const LoKi::Particles::DeltaMeasuredMass& right ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_eval ( right ) {} ;
// ============================================================================/
/** copy constructor
 *  @param right instance to be copied
 */
// ============================================================================
LoKi::Particles::AbsDeltaMeasuredMass::AbsDeltaMeasuredMass
( const LoKi::Particles::AbsDeltaMeasuredMass& right ) 
  : LoKi::AuxFunBase                      ( right ) 
  , LoKi::Function<const LHCb::Particle*> ( right )
  , m_eval ( right.m_eval  ) {} ;
// ============================================================================
/// destructor (virtual)
// ============================================================================
LoKi::Particles::AbsDeltaMeasuredMass::~AbsDeltaMeasuredMass(){}
// ============================================================================
/// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::AbsDeltaMeasuredMass*
LoKi::Particles::AbsDeltaMeasuredMass::clone() const
{ return new LoKi::Particles::AbsDeltaMeasuredMass( *this ) ; }
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::AbsDeltaMeasuredMass::result_type 
LoKi::Particles::AbsDeltaMeasuredMass::operator() 
  ( LoKi::Particles::AbsDeltaMeasuredMass::argument p ) const 
{
  if ( 0 != p ) { return ::fabs( m_eval ( p ) ) ; } // RETURN 
  Error(" Invalid Particle, return 'InvalidMass'");
  return LoKi::Constants::InvalidMass;                      // RETURN 
};
// ============================================================================
/// specific printout
// ============================================================================
std::ostream& 
LoKi::Particles::AbsDeltaMeasuredMass::fillStream( std::ostream& s ) const 
{ return s << "|" << m_eval << "|" ; };
// ============================================================================

// ============================================================================
LoKi::Particles::DeltaMeasuredMassChi2::DeltaMeasuredMassChi2 
( const double mass ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_eval ( mass ) {} ;
// ============================================================================
LoKi::Particles::DeltaMeasuredMassChi2::DeltaMeasuredMassChi2 
( const ParticleProperty& pp ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , m_eval ( pp   ) {} ;
// ============================================================================
LoKi::Particles::DeltaMeasuredMassChi2::DeltaMeasuredMassChi2 
( const std::string&    nam , 
  IParticlePropertySvc* svc )
  : LoKi::Function<const LHCb::Particle*> ()
  , m_eval ( nam , svc ) {} ;
// ============================================================================
LoKi::Particles::DeltaMeasuredMassChi2::DeltaMeasuredMassChi2 
( const LHCb::ParticleID&     pid , 
  IParticlePropertySvc* svc )
  : LoKi::Function<const LHCb::Particle*> ()
  , m_eval ( pid , svc ) {} ;
// ============================================================================
LoKi::Particles::DeltaMeasuredMassChi2::DeltaMeasuredMassChi2 
( const std::string&    nam )
  : LoKi::Function<const LHCb::Particle*> ()
  , m_eval ( nam       ) {} ;
// ============================================================================
LoKi::Particles::DeltaMeasuredMassChi2::DeltaMeasuredMassChi2 
( const LHCb::ParticleID&     pid )
  : LoKi::Function<const LHCb::Particle*> ()
  , m_eval ( pid       ) {} ;
// ============================================================================
LoKi::Particles::DeltaMeasuredMassChi2::DeltaMeasuredMassChi2 
( const LoKi::Particles::DeltaMeasuredMass& right )
  : LoKi::Function<const LHCb::Particle*> () 
  , m_eval ( right ) {} ;
// ============================================================================
LoKi::Particles::DeltaMeasuredMassChi2::DeltaMeasuredMassChi2 
( const LoKi::Particles::DeltaMeasuredMassChi2& right )
  : LoKi::AuxFunBase                      ( right ) 
  , LoKi::Function<const LHCb::Particle*> ( right ) 
  , m_eval ( right.m_eval ) {} ;
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
  return ::fabs ( m_eval( p ) ) / p -> measuredMassErr() ;
};
// ============================================================================
LoKi::Particles::DeltaMeasuredMassChi2*
LoKi::Particles::DeltaMeasuredMassChi2::clone() const 
 { return new DeltaMeasuredMassChi2( *this ) ; };
// ============================================================================
// specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::DeltaMeasuredMassChi2::fillStream( std::ostream& s ) const 
{ return s << "CHI2M["  << m0() <<  "]" ; }
// ============================================================================

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
};
// ============================================================================
std::ostream& 
LoKi::Particles::ConfidenceLevel::fillStream
( std::ostream& s ) const 
{ return s << "CL" ; }
// ============================================================================


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
};
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
};
// ============================================================================
std::ostream& 
LoKi::Particles::NumberOfDaughters::fillStream
( std::ostream& s ) const 
{ return s << "NDAUGS" ; }
// ============================================================================

// ============================================================================
LoKi::Particles::HasInfo::HasInfo( const int info ) 
  : LoKi::Predicate<const LHCb::Particle*>() 
  , m_info ( info ) 
{}
// ============================================================================
LoKi::Particles::HasInfo::HasInfo
( const LoKi::Particles::HasInfo& right ) 
  : LoKi::AuxFunBase                      ( right ) 
  , LoKi::Predicate<const LHCb::Particle*>( right ) 
  , m_info ( right.m_info )
{}
// ============================================================================
LoKi::Particles::HasInfo::~HasInfo(){}
// ============================================================================
LoKi::Particles::HasInfo* 
LoKi::Particles::HasInfo::clone() const 
{ return new HasInfo( *this ) ; }
// ============================================================================
LoKi::Particles::HasInfo::result_type 
LoKi::Particles::HasInfo::operator() 
  ( LoKi::Particles::HasInfo::argument p ) const 
{
  if ( 0 != p ) { return p -> hasInfo( m_info ) ; }      // RETURN 
  Error ( " Invalid Particle, return 'false'" ) ;
  return false;                                           // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Particles::HasInfo::fillStream
( std::ostream& s ) const 
{ return s << "HASINFO[" << m_info << "]" ; }
// ============================================================================
LoKi::Particles::Info::Info
( const int    key , 
  const double def , 
  const double bad )
  : LoKi::Function<const LHCb::Particle*>() 
  , m_key ( key ) 
  , m_def ( def ) 
  , m_bad ( bad ) 
{}
// ============================================================================
LoKi::Particles::Info::Info
( const int    key , 
  const double def )
  : LoKi::Function<const LHCb::Particle*>() 
  , m_key ( key ) 
  , m_def ( def ) 
  , m_bad ( def ) 
{}
// ============================================================================
LoKi::Particles::Info::Info
( const LoKi::Particles::Info& right ) 
  : LoKi::AuxFunBase                     ( right ) 
  , LoKi::Function<const LHCb::Particle*>( right ) 
  , m_key ( right.m_key  )
  , m_def ( right.m_def  )
  , m_bad ( right.m_bad  )
{}
// ============================================================================
LoKi::Particles::Info::~Info(){}
// ============================================================================
LoKi::Particles::Info* 
LoKi::Particles::Info::clone() const 
{ return new Info( *this ) ; }
// ============================================================================
LoKi::Particles::Info::result_type 
LoKi::Particles::Info::operator() 
  ( LoKi::Particles::Info::argument p ) const 
{
  if ( 0 != p ) { return p -> info( m_key , m_def ) ; }      // RETURN 
  Error ( " Invalid Particle, return " + Gaudi::Utils::toString( m_bad ) ) ;
  return m_bad;                                              // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Particles::Info::fillStream
( std::ostream& s ) const 
{ 
  s << "INFO[" << m_key << "," << m_def ;
  if ( m_bad != m_def ) { s << "," << m_bad ; }
  return s << "]" ;
}
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
};
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
};
// ============================================================================
std::ostream& 
LoKi::Particles::HasVertex::fillStream
( std::ostream& s ) const 
{ return s << "HASVERTEX" ; }
// ============================================================================
/// constructor from 3-vector 
// ============================================================================
LoKi::Particles::TransverseMomentumRel::TransverseMomentumRel
( const LoKi::ThreeVector&  v ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , m_momentum ( v ) 
{} ;
// ============================================================================
/// constructor from 4-vector 
// ============================================================================
LoKi::Particles::TransverseMomentumRel::TransverseMomentumRel
( const LoKi::LorentzVector&  v ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , m_momentum ( v ) 
{} ;
// ============================================================================
/// constructor from LHCb::Particle
// ============================================================================
LoKi::Particles::TransverseMomentumRel::TransverseMomentumRel
( const LHCb::Particle*  v ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , m_momentum () 
{
  if ( 0 == v ) { Exception ( "Invalid LHCb::Particle*!" ) ; }
  m_momentum = v->momentum() ;
} ;
// ============================================================================
/// copy constructor 
// ============================================================================
LoKi::Particles::TransverseMomentumRel::TransverseMomentumRel 
( const LoKi::Particles::TransverseMomentumRel& right ) 
  : LoKi::AuxFunBase                     ( right ) 
  , LoKi::Function<const LHCb::Particle*>( right ) 
  , m_momentum ( right.m_momentum ) 
{} ;
// ============================================================================
/// MANDATORY: clone method ("virtual constructor") 
// ============================================================================
LoKi::Particles::TransverseMomentumRel*
LoKi::Particles::TransverseMomentumRel::clone() const 
{ return new LoKi::Particles::TransverseMomentumRel(*this) ; }
// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::TransverseMomentumRel::~TransverseMomentumRel(){}
// ============================================================================
/// MANDATORY: the only one essential mehtod 
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
  return LoKi::Kinematics::transverseMomentumDir( p->momentum() , m_momentum );
} ;
// ============================================================================
/// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::TransverseMomentumRel::fillStream( std::ostream& s ) const 
{ return s << "PTDIR[" << m_momentum << "]" ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================

