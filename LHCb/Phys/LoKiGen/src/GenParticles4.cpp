// $Id$
// =============================================================================
// Include files 
// =============================================================================
// LHCbMath
// =============================================================================
#include "LHCbMath/LHCbMath.h"
// =============================================================================
// Local
// =============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Kinematics.h"
#include "LoKi/GenParticles4.h"
#include "LoKi/GenChildSelector.h"
// =============================================================================
// GaudiKernel
// =============================================================================
#include "GaudiKernel/ToStream.h"
// =============================================================================
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
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-28 
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /** @var s_equal
   *  comparison criteria for doubles 
   */
  const LHCb::Math::Equal_To<double> s_equal ; 
  // ==========================================================================
}
// ============================================================================
// constructor from one selector
// ============================================================================
LoKi::GenParticles::FourMomentum::FourMomentum
( const LoKi::GenChild::Selector& c1 )
  : LoKi::GenTypes::GFunc  () 
  , m_children ( 1 , c1 ) 
{}
// ============================================================================
// constructor from two selectors 
// ============================================================================
LoKi::GenParticles::FourMomentum::FourMomentum
( const LoKi::GenChild::Selector& c1 ,
  const LoKi::GenChild::Selector& c2 ) 
  : LoKi::GenTypes::GFunc  () 
  , m_children () 
{
  m_children . push_back ( c1 ) ;
  m_children . push_back ( c2 ) ;
}
// ============================================================================
// constructor from three selectors 
// ============================================================================
LoKi::GenParticles::FourMomentum::FourMomentum
( const LoKi::GenChild::Selector& c1 ,
  const LoKi::GenChild::Selector& c2 ,
  const LoKi::GenChild::Selector& c3 ) 
  : LoKi::GenTypes::GFunc  () 
  , m_children () 
{
  m_children . push_back ( c1 ) ;
  m_children . push_back ( c2 ) ;
  m_children . push_back ( c3 ) ;
}
// ============================================================================
// constructor from four selectors 
// ============================================================================
LoKi::GenParticles::FourMomentum::FourMomentum
( const LoKi::GenChild::Selector& c1 ,
  const LoKi::GenChild::Selector& c2 ,
  const LoKi::GenChild::Selector& c3 ,
  const LoKi::GenChild::Selector& c4 ) 
  : LoKi::GenTypes::GFunc  () 
  , m_children () 
{
  m_children . push_back ( c1 ) ;
  m_children . push_back ( c2 ) ;
  m_children . push_back ( c3 ) ;
  m_children . push_back ( c4 ) ;
}
// ============================================================================
// constructor from list of  selectors 
// ============================================================================
LoKi::GenParticles::FourMomentum::FourMomentum
( const LoKi::GenChild::Selector::Vector& cs )
  : LoKi::GenTypes::GFunc  () 
  , m_children ( cs ) 
{}
// ============================================================================
// constructor from one selector
// ============================================================================
LoKi::GenParticles::FourMomentum::FourMomentum
( const LoKi::GenTypes::GCuts& c1 )
  : LoKi::GenTypes::GFunc  () 
  , m_children ( 1 , c1 ) 
{}
// ============================================================================
// constructor from two selectors 
// ============================================================================
LoKi::GenParticles::FourMomentum::FourMomentum
( const LoKi::GenTypes::GCuts& c1 ,
  const LoKi::GenTypes::GCuts& c2 )
  : LoKi::GenTypes::GFunc  () 
  , m_children () 
{
  m_children . push_back ( c1 ) ;
  m_children . push_back ( c2 ) ;
}
// ============================================================================
// constructor from three selectors 
// ============================================================================
LoKi::GenParticles::FourMomentum::FourMomentum
( const LoKi::GenTypes::GCuts& c1 ,
  const LoKi::GenTypes::GCuts& c2 ,
  const LoKi::GenTypes::GCuts& c3 )
  : LoKi::GenTypes::GFunc  () 
  , m_children () 
{
  m_children . push_back ( c1 ) ;
  m_children . push_back ( c2 ) ;
  m_children . push_back ( c3 ) ;
}
// ============================================================================
// constructor from four selectors 
// ============================================================================
LoKi::GenParticles::FourMomentum::FourMomentum
( const LoKi::GenTypes::GCuts& c1 ,
  const LoKi::GenTypes::GCuts& c2 ,
  const LoKi::GenTypes::GCuts& c3 ,
  const LoKi::GenTypes::GCuts& c4 )
  : LoKi::GenTypes::GFunc  () 
  , m_children () 
{
  m_children . push_back ( c1 ) ;
  m_children . push_back ( c2 ) ;
  m_children . push_back ( c3 ) ;
  m_children . push_back ( c4 ) ;
}
// ============================================================================
// default constructo 
// ============================================================================
LoKi::GenParticles::FourMomentum::FourMomentum()
  : LoKi::GenTypes::GFunc  () 
  , m_children ()
{}  
// ============================================================================
// base class printput 
// ============================================================================
std::ostream& LoKi::GenParticles::FourMomentum::print_ 
( std::ostream&      s  , 
  const std::string& n1 , 
  const std::string& n2 ) const 
{
  //
  if ( m_children.empty() ) { return s << n2 ; } // RETURN 
  //
  typedef  LoKi::GenChild::Selector::Vector::const_iterator CI ;
  //
  if ( 4 < m_children.size() )
  {
    s << n1 << "([ "  ;
    //
    for (  CI ii =m_children.begin() ; m_children.end() != ii ; ++ii ) 
    {
      if ( m_children.begin() != ii ) { s << "," ; }
      s << (*ii) ;
    }
    //
    return s <<  " ]) " ;                                 // REUTRN
  }
  //
  s << n1 << "(" ;
  for (  CI ii = m_children.begin() ; m_children.end() != ii ; ++ii ) 
  {
    if ( m_children.begin() != ii ) { s << "," ; }
    s << (*ii) ;
  }
  //
  return s << ")" ;
}
// ============================================================================
// evaluate the full four momentum of combination
// ============================================================================
LoKi::LorentzVector 
LoKi::GenParticles::FourMomentum::momentum 
( const HepMC::GenParticle* p ) const 
{
  if ( 0 == p ) 
  {
    Error ("HepMC::GenParticle* points to NULL, return empty 4-momentum") ;
    return LoKi::LorentzVector () ;
  }
  //
  if ( m_children.empty() ) { return LoKi::LorentzVector ( p-> momentum() ) ; }
  //
  LoKi::LorentzVector result = LoKi::LorentzVector() ;
  //
  for (  LoKi::GenChild::Selector::Vector::const_iterator ii = m_children.begin() ; 
         m_children.end() != ii ; ++ii ) 
  {
    const HepMC::GenParticle* c = ii->child ( p ) ;
    if ( 0 == c ) 
    {
      Error ( "daughter HepMC::GenParticle* points to NULL, skip it" ) ;
      continue ;
    }
    result += LoKi::LorentzVector ( c->momentum () ) ;
  }
  //
  return result ;
}
// ============================================================================
// get the rapidity 
// ============================================================================
double LoKi::GenParticles::FourMomentum::y  
( const LoKi::LorentzVector& lv ) const 
{
  const double e  = lv . E  () ;
  const double pz = lv . Pz () ;
  //
  return 0.5 * std::log( ( e + pz ) / ( e - pz ) ) ;
}
// ============================================================================
// get the rapidity 
// ============================================================================
double LoKi::GenParticles::FourMomentum::y0
( const LoKi::LorentzVector& lv ) const 
{
  const double e  = lv . E  () ;
  const double p  = lv . P  () ;
  //
  return 0.5 * std::log( ( e + p ) / ( e - p ) ) ;
}
// ============================================================================
// get the phi
// ============================================================================
double LoKi::GenParticles::FourMomentum::phi 
( const LoKi::LorentzVector& lv ) const { return adjust ( lv.phi () ) ; }
// ============================================================================
// get the eta
// ============================================================================
double LoKi::GenParticles::FourMomentum::eta
( const LoKi::LorentzVector& lv ) const { return lv . Eta() ; }
// ==========================================================================
// get the beta
// ============================================================================
double LoKi::GenParticles::FourMomentum::beta
( const LoKi::LorentzVector& lv ) const { return lv . Beta  () ; }
// ==========================================================================
// get the gamma
// ============================================================================
double LoKi::GenParticles::FourMomentum::gamma
( const LoKi::LorentzVector& lv ) const { return lv . Gamma () ; }
// ==========================================================================
// get the beta*gamma
// ============================================================================
double LoKi::GenParticles::FourMomentum::bgamma
( const LoKi::LorentzVector& lv ) const 
{ 
  //
  const double m = lv.M () ;
  //
  if ( s_equal ( m , 0 ) ) { return LoKi::Constants::PositiveInfinity ; }
  //
  return lv.P () / m ;  
}
// ==========================================================================


// ============================================================================
// adjust delta phi into the range of [-180:180]degrees
// ============================================================================
double  LoKi::GenParticles::FourMomentum::adjust ( double angle ) const 
{ 
  static const double s_180 = 180 * Gaudi::Units::degree ;
  static const double s_360 = 360 * Gaudi::Units::degree ;
  //
  while ( angle >       s_180 ) { angle -= s_360 ; }
  while ( angle <  -1 * s_180 ) { angle += s_360 ; }
  //
  return angle ;
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::Mass::~Mass(){}
// ============================================================================
// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::GenParticles::Mass*
LoKi::GenParticles::Mass::clone() const 
{ return new LoKi::GenParticles::Mass(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::Mass::result_type 
LoKi::GenParticles::Mass::operator()
  ( LoKi::GenParticles::Mass::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ("HepMC::GenParticle* points to NULL, return Invalid Mass") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  return momentum (  p ) . M () ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& 
LoKi::GenParticles::Mass::fillStream ( std::ostream& s ) const 
{ return print_ ( s , "GMASS" , "GM" ) ; }
// ============================================================================



// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::Energy::~Energy(){}
// ============================================================================
// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::GenParticles::Energy*
LoKi::GenParticles::Energy::clone() const 
{ return new LoKi::GenParticles::Energy(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::Energy::result_type 
LoKi::GenParticles::Energy::operator()
  ( LoKi::GenParticles::Energy::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ("HepMC::GenParticle* points to NULL, return Invalid Energy") ;
    return LoKi::Constants::InvalidEnergy ;
  }
  //
  return momentum (  p ) . E () ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& 
LoKi::GenParticles::Energy::fillStream ( std::ostream& s ) const 
{ return print_ ( s , "GENERGY" , "GE" ) ; }



// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::Momentum::~Momentum(){}
// ============================================================================
// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::GenParticles::Momentum*
LoKi::GenParticles::Momentum::clone() const 
{ return new LoKi::GenParticles::Momentum(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::Momentum::result_type 
LoKi::GenParticles::Momentum::operator()
  ( LoKi::GenParticles::Momentum::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ("HepMC::GenParticle* points to NULL, return Invalid Momentum") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  //
  return momentum (  p ) . P () ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& 
LoKi::GenParticles::Momentum::fillStream ( std::ostream& s ) const 
{ return print_ ( s , "GMOMENTUM" , "GP" ) ; }


// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::MomentumX::~MomentumX(){}
// ============================================================================
// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::GenParticles::MomentumX*
LoKi::GenParticles::MomentumX::clone() const 
{ return new LoKi::GenParticles::MomentumX(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::MomentumX::result_type 
LoKi::GenParticles::MomentumX::operator()
  ( LoKi::GenParticles::MomentumX::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ("HepMC::GenParticle* points to NULL, return Invalid Momentum") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  //
  return momentum (  p ) . Px () ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& 
LoKi::GenParticles::MomentumX::fillStream ( std::ostream& s ) const 
{ return print_ ( s , "GMOMX" , "GPX" ) ; }

// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::MomentumY::~MomentumY(){}
// ============================================================================
// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::GenParticles::MomentumY*
LoKi::GenParticles::MomentumY::clone() const 
{ return new LoKi::GenParticles::MomentumY(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::MomentumY::result_type 
LoKi::GenParticles::MomentumY::operator()
  ( LoKi::GenParticles::MomentumY::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ("HepMC::GenParticle* points to NULL, return Invalid Momentum") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  //
  return momentum (  p ) . Py () ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& 
LoKi::GenParticles::MomentumY::fillStream ( std::ostream& s ) const 
{ return print_ ( s , "GMOMY" , "GPY" ) ; }


// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::MomentumZ::~MomentumZ(){}
// ============================================================================
// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::GenParticles::MomentumZ*
LoKi::GenParticles::MomentumZ::clone() const 
{ return new LoKi::GenParticles::MomentumZ(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::MomentumZ::result_type 
LoKi::GenParticles::MomentumZ::operator()
  ( LoKi::GenParticles::MomentumZ::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ("HepMC::GenParticle* points to NULL, return Invalid Momentum") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  //
  return momentum (  p ) . Pz () ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& 
LoKi::GenParticles::MomentumZ::fillStream ( std::ostream& s ) const 
{ return print_ ( s , "GMOMZ" , "GPZ" ) ; }



// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::TransverseMomentum::~TransverseMomentum(){}
// ============================================================================
// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::GenParticles::TransverseMomentum*
LoKi::GenParticles::TransverseMomentum::clone() const 
{ return new LoKi::GenParticles::TransverseMomentum(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::TransverseMomentum::result_type 
LoKi::GenParticles::TransverseMomentum::operator()
  ( LoKi::GenParticles::TransverseMomentum::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ("HepMC::GenParticle* points to NULL, return Invalid Momentum") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  //
  return momentum (  p ) . Pt () ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& 
LoKi::GenParticles::TransverseMomentum::fillStream ( std::ostream& s ) const 
{ return print_ ( s , "GMOMT" , "GPT" ) ; }



// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::PseudoRapidity::~PseudoRapidity(){}
// ============================================================================
// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::GenParticles::PseudoRapidity*
LoKi::GenParticles::PseudoRapidity::clone() const 
{ return new LoKi::GenParticles::PseudoRapidity(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::PseudoRapidity::result_type 
LoKi::GenParticles::PseudoRapidity::operator()
  ( LoKi::GenParticles::PseudoRapidity::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ("HepMC::GenParticle* points to NULL, return NegativeInfinity") ;
    return LoKi::Constants::NegativeInfinity;
  }
  //
  return eta ( momentum (  p ) ) ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& 
LoKi::GenParticles::PseudoRapidity::fillStream ( std::ostream& s ) const 
{ return print_ ( s , "GPSEUDORAPIDITY" , "GETA" ) ; }


// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::Rapidity::~Rapidity(){}
// ============================================================================
// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::GenParticles::Rapidity*
LoKi::GenParticles::Rapidity::clone() const 
{ return new LoKi::GenParticles::Rapidity(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::Rapidity::result_type 
LoKi::GenParticles::Rapidity::operator()
  ( LoKi::GenParticles::Rapidity::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ("HepMC::GenParticle* points to NULL, return NegativeInfinity") ;
    return LoKi::Constants::NegativeInfinity;
  }
  //
  return y ( momentum (  p ) ) ;
  //
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& 
LoKi::GenParticles::Rapidity::fillStream ( std::ostream& s ) const 
{ return print_ ( s , "GRAPIDITY" , "GY" ) ; }


// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::Rapidity0::~Rapidity0(){}
// ============================================================================
// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::GenParticles::Rapidity0*
LoKi::GenParticles::Rapidity0::clone() const 
{ return new LoKi::GenParticles::Rapidity0(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::Rapidity0::result_type 
LoKi::GenParticles::Rapidity0::operator()
  ( LoKi::GenParticles::Rapidity0::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ("HepMC::GenParticle* points to NULL, return NegativeInfinity") ;
    return LoKi::Constants::NegativeInfinity;
  }
  //
  return y0 ( momentum (  p ) ) ;
  //
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& 
LoKi::GenParticles::Rapidity0::fillStream ( std::ostream& s ) const 
{ return print_ ( s , "GRAPIDITY0" , "GY0" ) ; }


// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::Phi::~Phi (){}
// ============================================================================
// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::GenParticles::Phi*
LoKi::GenParticles::Phi::clone() const 
{ return new LoKi::GenParticles::Phi(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::Phi::result_type 
LoKi::GenParticles::Phi::operator()
  ( LoKi::GenParticles::Phi::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* points to NULL, return -100 ") ;
    return -100 ;
  }
  //
  return phi ( momentum (  p ) ) ;
  //
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& 
LoKi::GenParticles::Phi::fillStream ( std::ostream& s ) const 
{ return print_ ( s , "GASYMUTH" , "GPHI" ) ; }
// ============================================================================

// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::Theta::~Theta (){}
// ============================================================================
// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::GenParticles::Theta*
LoKi::GenParticles::Theta::clone() const 
{ return new LoKi::GenParticles::Theta(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::Theta::result_type 
LoKi::GenParticles::Theta::operator()
  ( LoKi::GenParticles::Theta::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* points to NULL, return -100 ") ;
    return -100 ;
  }
  //
  return momentum (  p ) . Theta () ;  
  //
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& 
LoKi::GenParticles::Theta::fillStream ( std::ostream& s ) const 
{ return print_ ( s , "GPOLAR" , "GTHETA" ) ; }




// ============================================================================
// default constructor
// ============================================================================
LoKi::GenParticles::Beta::Beta() 
  : LoKi::GenTypes::GFunc () 
{} 
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::Beta::~Beta(){}
// ============================================================================
// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::GenParticles::Beta*
LoKi::GenParticles::Beta::clone() const 
{ return new LoKi::GenParticles::Beta ( *this ) ; }
// ============================================================================
// get beta-factor 
// ============================================================================
double LoKi::GenParticles::Beta::beta   ( const HepMC::GenParticle* p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* points to NULL, return -1 ") ;
    return -1 ;
  }
  //
  return LoKi::LorentzVector ( p->momentum() ).Beta() ;
  //
}
// ============================================================================
// get gamma-factor 
// ============================================================================
double LoKi::GenParticles::Beta::gamma   ( const HepMC::GenParticle* p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* points to NULL, return -1 ") ;
    return -1 ;
  }
  //
  const LoKi::LorentzVector lv ( p->momentum() ) ;
  //
  const double m = lv.M () ;
  const double E = lv.E () ;
  //
  if ( s_equal ( E , m ) ) { return 1 ; } 
  //
  if ( s_equal ( m , 0 ) ) 
  {
    //
    //
    Error ( "Light-like particle, return +infinity") ;
    return LoKi::Constants::PositiveInfinity ;
  }
  //
  return E / m ;
  //
}
// ============================================================================
// get gamma-factor 
// ============================================================================
double LoKi::GenParticles::Beta::betagamma   ( const HepMC::GenParticle* p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* points to NULL, return -1 ") ;
    return -1 ;
  }
  //
  const LoKi::LorentzVector lv ( p->momentum() ) ;
  const double P = lv.P () ;
  if ( s_equal ( P , 0 ) ) { return 0 ; }
  //
  const double m = lv.M () ;
  if ( s_equal ( m , 0 ) ) 
  {
    //
    Error ( "Light-like particle, return +infinity") ;
    return LoKi::Constants::PositiveInfinity ;
  }
  //
  return P / m ;
  //
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::Beta::result_type 
LoKi::GenParticles::Beta::operator()
  ( LoKi::GenParticles::Beta::argument p ) const { return beta ( p ) ; }
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& 
LoKi::GenParticles::Beta::fillStream ( std::ostream& s ) const 
{ return s << "GBETA"; }



// ============================================================================
// default constructor
// ============================================================================
LoKi::GenParticles::Gamma::Gamma() 
  : LoKi::GenParticles::Beta () 
{} 
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::Gamma::~Gamma(){}
// ============================================================================
// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::GenParticles::Gamma*
LoKi::GenParticles::Gamma::clone() const 
{ return new LoKi::GenParticles::Gamma ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::Gamma::result_type 
LoKi::GenParticles::Gamma::operator()
  ( LoKi::GenParticles::Gamma::argument p ) const { return gamma ( p ) ; }
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& 
LoKi::GenParticles::Gamma::fillStream ( std::ostream& s ) const 
{ return s << "GGAMMA"; }


// ============================================================================
// default constructor
// ============================================================================
LoKi::GenParticles::BetaGamma::BetaGamma() 
  : LoKi::GenParticles::Beta () 
{} 
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::BetaGamma::~BetaGamma(){}
// ============================================================================
// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::GenParticles::BetaGamma*
LoKi::GenParticles::BetaGamma::clone() const 
{ return new LoKi::GenParticles::BetaGamma ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::BetaGamma::result_type 
LoKi::GenParticles::BetaGamma::operator()
  ( LoKi::GenParticles::BetaGamma::argument p ) const { return betagamma ( p ) ; }
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& 
LoKi::GenParticles::BetaGamma::fillStream ( std::ostream& s ) const 
{ return s << "GBETAGAMMA"; }
// ============================================================================



// ============================================================================
//  constructor from the angle
// ============================================================================
LoKi::GenParticles::DeltaPhi::DeltaPhi ( const double phi ) 
  : LoKi::GenParticles::Phi () 
  , m_phi  ( phi )
{ 
  m_phi = adjust ( m_phi ) ; 
} 
// ============================================================================
//  constructor from the vector 
// ============================================================================
LoKi::GenParticles::DeltaPhi::DeltaPhi ( const LoKi::ThreeVector& v ) 
  : LoKi::GenParticles::Phi () 
  , m_phi  ( v.Phi() )
{ 
  m_phi = adjust ( m_phi ) ; 
}  
// ============================================================================
//  constructor from the vector 
// ============================================================================
LoKi::GenParticles::DeltaPhi::DeltaPhi ( const LoKi::LorentzVector& v ) 
  : LoKi::GenParticles::Phi () 
  , m_phi  ( v.Phi() )
{ 
  m_phi = adjust ( m_phi ) ; 
}  
// ============================================================================
//  constructor from the particle
// ============================================================================
LoKi::GenParticles::DeltaPhi::DeltaPhi ( const HepMC::GenParticle* p )
  : LoKi::GenParticles::Phi () 
  , m_phi  ( )
{ 
  if ( 0 == p ) { Exception("Invalid HepMC::GenParticle*") ;}
  m_phi = p->momentum().phi() ;
  m_phi = adjust ( m_phi ) ; 
}  
// ============================================================================
//  MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::GenParticles::DeltaPhi*
LoKi::GenParticles::DeltaPhi::clone() const
{ return new LoKi::GenParticles::DeltaPhi(*this) ; }
// ============================================================================
//  MANDATORY: the only essential method 
// ============================================================================
LoKi::GenParticles::DeltaPhi::result_type
LoKi::GenParticles::DeltaPhi::operator() 
  ( LoKi::GenParticles::DeltaPhi::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* points to NULL, return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  return dphi ( p )  ;
} 
// ============================================================================
double LoKi::GenParticles::DeltaPhi::dphi 
( LoKi::GenParticles::DeltaPhi::argument p ) const 
{ return adjust ( phi ( momentum ( p )  ) - phi0 () ) ; }
// ============================================================================
//  OPTIONAL: "SHORT" representation
// ============================================================================
std::ostream& LoKi::GenParticles::DeltaPhi::fillStream ( std::ostream& s ) const 
{ return s << "GDPHI(" << phi0() << ")" ; }
// ============================================================================
//  constructor from the angle
// ============================================================================
LoKi::GenParticles::DeltaEta::DeltaEta ( const double eta ) 
  : LoKi::GenParticles::PseudoRapidity () 
  , m_eta  ( eta )
{}
// ============================================================================
//  constructor from the vector 
// ============================================================================
LoKi::GenParticles::DeltaEta::DeltaEta ( const LoKi::ThreeVector& v ) 
  : LoKi::GenParticles::PseudoRapidity () 
  , m_eta  ( v.Eta() )
{}
// ============================================================================
//  constructor from the vector 
// ============================================================================
LoKi::GenParticles::DeltaEta::DeltaEta ( const LoKi::LorentzVector& v ) 
  : LoKi::GenParticles::PseudoRapidity () 
  , m_eta  ( v.Eta() )
{}
// ============================================================================
//  constructor from the particle
// ============================================================================
LoKi::GenParticles::DeltaEta::DeltaEta ( const HepMC::GenParticle* p ) 
  : LoKi::GenParticles::PseudoRapidity () 
  , m_eta  ( )
{
  if ( 0 == p ) { Exception("Invalid HepMC::GenParticle*") ;}
  m_eta = p->momentum().pseudoRapidity() ;
} 
// ============================================================================
//  MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::GenParticles::DeltaEta* 
LoKi::GenParticles::DeltaEta::clone() const 
{ return new LoKi::GenParticles::DeltaEta(*this) ; }
// ============================================================================
//  MANDATORY: the only essential method 
// ============================================================================
LoKi::GenParticles::DeltaEta::result_type
LoKi::GenParticles::DeltaEta::operator() 
  ( LoKi::GenParticles::DeltaEta::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ("HepMC::GenParticle* points to NULL, return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  return deta ( p ) ;
} 
// ============================================================================
double LoKi::GenParticles::DeltaEta::deta 
( LoKi::GenParticles::DeltaEta::argument p ) const 
{ return eta ( momentum ( p )  ) - eta0 () ; }
// ============================================================================
//  OPTIONAL: "SHORT" representation
// ============================================================================
std::ostream& LoKi::GenParticles::DeltaEta::fillStream ( std::ostream& s ) const 
{ return s << "GDETA(" << m_eta << ")" ; }
// ============================================================================
//  constructor from eta and phi
// ============================================================================
LoKi::GenParticles::DeltaR2::DeltaR2 ( const double eta , const double phi ) 
  : LoKi::GenParticles::DeltaPhi ( phi ) 
  , m_deta ( eta ) 
{}
// ============================================================================
//  constructor from the vector 
// ============================================================================
LoKi::GenParticles::DeltaR2::DeltaR2 ( const LoKi::ThreeVector& v ) 
  : LoKi::GenParticles::DeltaPhi ( v ) 
  , m_deta ( v ) 
{}
// ============================================================================
//  constructor from the vector 
// ============================================================================
LoKi::GenParticles::DeltaR2::DeltaR2 ( const LoKi::LorentzVector& v ) 
  : LoKi::GenParticles::DeltaPhi ( v ) 
  , m_deta ( v ) 
{}
// ============================================================================
//  constructor from the particle
// ============================================================================
LoKi::GenParticles::DeltaR2::DeltaR2 ( const HepMC::GenParticle* v ) 
  : LoKi::GenParticles::DeltaPhi ( v ) 
  , m_deta ( v ) 
{}
// ============================================================================
//  MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::GenParticles::DeltaR2*
LoKi::GenParticles::DeltaR2::clone() const 
{ return new LoKi::GenParticles::DeltaR2(*this) ; }
// ============================================================================
//  MANDATORY: the only essential method 
// ============================================================================
LoKi::GenParticles::DeltaR2::result_type
LoKi::GenParticles::DeltaR2::operator() 
  ( LoKi::GenParticles::DeltaR2::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ("HepMC::GenParticle* points to NULL, return 'InvalidDustance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  //
  const double dphi_ = dphi ( p ) ;
  const double deta_ = deta ( p ) ;
  //
  return dphi_*dphi_ + deta_*deta_ ;
} 
// ============================================================================
//  OPTIONAL: "SHORT" representation
// ============================================================================
std::ostream& LoKi::GenParticles::DeltaR2::fillStream ( std::ostream& s ) const 
{ return s << "GDR2( " << eta0() << "," << phi0() << ")" ; }

// ============================================================================
// The END 
// ============================================================================
