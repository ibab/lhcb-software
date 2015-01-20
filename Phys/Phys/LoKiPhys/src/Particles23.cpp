// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/ParticleProperty.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Particles23.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/GetTools.h"
// ============================================================================
// DaVinciInterfaces
// ============================================================================
#include "Kernel/IParticleTransporter.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/lexical_cast.hpp"
// ============================================================================
/** @file 
 *  implementation fiel for functord form the file LoKi/Particles23.h
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-10-13
 */
// ============================================================================
LoKi::Particles::WrongMass::WrongMass
( const double m1 ,
  const double m2 , 
  const IParticleTransporter* t  , 
  const double                dz )
  : LoKi::AuxFunBase ( std::tie ( m1 ,m2 , dz ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  () 
//
  , m_transporter ( t  ) 
  , m_dz          ( dz )
//
{
  m_masses.push_back ( m1 ) ; 
  m_masses.push_back ( m2 ) ;
}
// ============================================================================
LoKi::Particles::WrongMass::WrongMass
( const double m1 ,
  const double m2 ,
  const double m3 ,
  const IParticleTransporter* t  , 
  const double                dz ) 
  : LoKi::AuxFunBase ( std::tie ( m1 , m2 , m3 , dz ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  () 
//
  , m_transporter ( t  ) 
  , m_dz          ( dz )
//
{
  m_masses.push_back ( m1 ) ; 
  m_masses.push_back ( m2 ) ;
  m_masses.push_back ( m3 ) ;
}
// ============================================================================
LoKi::Particles::WrongMass::WrongMass
( const double m1 ,
  const double m2 ,
  const double m3 ,
  const double m4 ,
  const IParticleTransporter* t  , 
  const double                dz ) 
  : LoKi::AuxFunBase ( std::tie ( m1 , m2 , m3 , m4 , dz ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  () 
//
  , m_transporter ( t  ) 
  , m_dz          ( dz )
//
{
  m_masses.push_back ( m1 ) ; 
  m_masses.push_back ( m2 ) ;
  m_masses.push_back ( m3 ) ;
  m_masses.push_back ( m4 ) ;
}
// ============================================================================
LoKi::Particles::WrongMass::WrongMass
( const std::vector<double>& masses ,
  const IParticleTransporter* t  , 
  const double                dz ) 
  : LoKi::AuxFunBase ( std::tie ( masses , dz ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses ( masses ) 
  , m_pids   () 
  , m_names  () 
//
  , m_transporter ( t  ) 
  , m_dz          ( dz )
//
{}
// ============================================================================
LoKi::Particles::WrongMass::WrongMass 
( const LHCb::ParticleID& p1 , 
  const LHCb::ParticleID& p2 ,
  const double                dz ,
  const IParticleTransporter* t  ) 
  : LoKi::AuxFunBase ( std::tie ( p1 , p2  , dz ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  () 
//
  , m_transporter ( t  ) 
  , m_dz          ( dz )
//
{
  m_pids.push_back ( p1 ) ;
  m_pids.push_back ( p2 ) ;  
  decode().ignore () ;
}
// ============================================================================
LoKi::Particles::WrongMass::WrongMass 
( const LHCb::ParticleID& p1 , 
  const LHCb::ParticleID& p2 ,
  const LHCb::ParticleID& p3 ,
  const double                dz ,
  const IParticleTransporter* t  ) 
  : LoKi::AuxFunBase ( std::tie ( p1 , p2 , p3 , dz ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  () 
//
  , m_transporter ( t  ) 
  , m_dz          ( dz )
//
{
  m_pids.push_back ( p1 ) ;
  m_pids.push_back ( p2 ) ;  
  m_pids.push_back ( p3 ) ;  
  decode().ignore () ;
}
// ============================================================================
LoKi::Particles::WrongMass::WrongMass 
( const LHCb::ParticleID& p1 , 
  const LHCb::ParticleID& p2 ,
  const LHCb::ParticleID& p3 ,  
  const LHCb::ParticleID& p4 ,
  const double                dz ,
  const IParticleTransporter* t  ) 
  : LoKi::AuxFunBase ( std::tie ( p1 , p2 , p3 , p4 , dz ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  () 
//
  , m_transporter ( t  ) 
  , m_dz          ( dz )
//
{
  m_pids.push_back ( p1 ) ;
  m_pids.push_back ( p2 ) ;  
  m_pids.push_back ( p3 ) ;
  m_pids.push_back ( p4 ) ;  
  decode().ignore () ;
}
// ============================================================================
LoKi::Particles::WrongMass::WrongMass 
( const std::vector<LHCb::ParticleID>& pids ,
  const double                dz ,
  const IParticleTransporter* t  ) 
  : LoKi::AuxFunBase ( std::tie ( pids  , dz ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   ( pids ) 
  , m_names  () 
//
  , m_transporter ( t  ) 
  , m_dz          ( dz )
//
{
  decode().ignore () ;
}
// ============================================================================
LoKi::Particles::WrongMass::WrongMass 
( const std::string& p1 , 
  const std::string& p2 ,
  const double                dz ,
  const IParticleTransporter* t  )
  : LoKi::AuxFunBase ( std::tie ( p1 , p2   , dz ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  () 
//
  , m_transporter ( t  ) 
  , m_dz          ( dz )
//
{
  m_names.push_back ( p1 ) ;
  m_names.push_back ( p2 ) ;  
  decode().ignore () ;
}
// ============================================================================
LoKi::Particles::WrongMass::WrongMass 
( const std::string& p1 , 
  const std::string& p2 ,
  const std::string& p3 ,
  const double                dz ,
  const IParticleTransporter* t  ) 
  : LoKi::AuxFunBase ( std::tie ( p1 , p2 , p3 , dz ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  () 
//
  , m_transporter ( t  ) 
  , m_dz          ( dz )
//
{
  m_names.push_back ( p1 ) ;
  m_names.push_back ( p2 ) ;  
  m_names.push_back ( p3 ) ;  
  decode().ignore () ;
}
// ============================================================================
LoKi::Particles::WrongMass::WrongMass 
( const std::string& p1 , 
  const std::string& p2 ,
  const std::string& p3 , 
  const std::string& p4 ,
  const double                dz ,
  const IParticleTransporter* t  ) 
  : LoKi::AuxFunBase ( std::tie ( p1 , p2 , p3 , p4 , dz ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  () 
//
  , m_transporter ( t  ) 
  , m_dz          ( dz )
//
{
  m_names.push_back ( p1 ) ;
  m_names.push_back ( p2 ) ;  
  m_names.push_back ( p3 ) ;  
  m_names.push_back ( p4 ) ;  
  decode().ignore () ;
}
// ============================================================================
LoKi::Particles::WrongMass::WrongMass 
( const std::vector<std::string>& names ,
  const double                dz ,
  const IParticleTransporter* t  ) 
  : LoKi::AuxFunBase ( std::tie ( names , dz ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  ( names ) 
//
  , m_transporter ( t  ) 
  , m_dz          ( dz )
//
{
  decode().ignore () ;
}
// ============================================================================
// decode the masses 
// ============================================================================
StatusCode LoKi::Particles::WrongMass::decode () const 
{
  if ( !m_pids.empty() ) 
  {
    // decode pids into masses 
    m_masses.clear() ;
    typedef std::vector<LHCb::ParticleID> PIDs ;
    for ( PIDs::const_iterator ipid = m_pids.begin() ; 
          m_pids.end () != ipid ; ++ipid )
    {
      const LHCb::ParticleProperty* pp = LoKi::Particles::_ppFromPID ( *ipid ) ;
      if ( 0 == pp ) 
      {
        m_masses.clear() ;
        Warning ("decode(): invalid pid " + 
                 boost::lexical_cast<std::string>( ipid->pid() ) ) ;
        return StatusCode::FAILURE ;
      }
      m_masses. push_back ( pp->mass() ) ;  
    }
  }
  else if ( !m_names.empty() ) 
  {
    // decode pids into masses 
    m_masses.clear() ;
    typedef std::vector<std::string> Names ;
    for ( Names::const_iterator iname = m_names.begin() ; 
          m_names.end () != iname ; ++iname )
    {
      const LHCb::ParticleProperty* pp = LoKi::Particles::_ppFromName ( *iname ) ;
      if ( 0 == pp ) 
      {
        m_masses.clear() ;
        Warning ("decode(): invalid name '" + (*iname) + "'" ) ;
        return StatusCode::FAILURE ;
      }
      m_masses. push_back ( pp->mass() ) ;  
    }
  }
  //
  if ( m_masses.empty() ) 
  { 
    Warning ( "decode(): empty lits of mass hypotheses" ) ; 
    return StatusCode::FAILURE ;
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// evaluate the wrong mass 
// ============================================================================
LoKi::Particles::WrongMass::result_type 
LoKi::Particles::WrongMass::wmass  
( LoKi::Particles::WrongMass::argument p ) const
{
  StatusCode sc = check ( p ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from check(), return InvalidMass" , sc ) ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  typedef SmartRefVector<LHCb::Particle> DAUGS ;
  const DAUGS&           daughters = p -> daughters()      ;
  const Gaudi::XYZPoint& refPoint  = p -> referencePoint() ;
  // check for the tolerance 
  bool fast = true ;
  for ( DAUGS::const_iterator id = daughters.begin() ; 
        daughters.end() != id ; ++id ) 
  {
    const LHCb::Particle* dau = *id ;
    if ( 0 == dau ) 
    { 
      Error ("Daughter particle points to NULL, return InvalidMass") ; 
      return LoKi::Constants::InvalidMass ;                       // RETURN
    }
    if ( std::fabs ( dau->referencePoint().Z() - refPoint.Z() ) > m_dz ) 
    { 
      fast = false ;   
      break ;                                                // BREAK 
    }
  }
  // use fast algorithm:
  if ( fast ) { return wmass ( daughters ) ; }
  //
  
  // use the algorithm with the extrapolation 
  if ( !m_transporter ) 
  { m_transporter = LoKi::GetTools::particleTransporter ( *this ) ; }
  Assert ( !(!m_transporter) , "Unable to locate the Particle Transporter" ) ;
  //
  
  LoKi::LorentzVector sum  = LoKi::LorentzVector() ;
  //
  for ( DAUGS::const_iterator id1 = daughters.begin() ; 
        daughters.end() != id1 ; ++id1 ) 
  {
    //
    const std::size_t index   = id1 - daughters.begin() ;
    const double      newmass = m_masses[index] ;
    //
    const LHCb::Particle* dau = *id1 ;
    if ( 0 == dau ) 
    { 
      Error ("Daughter particle points to NULL") ; 
      continue ;                                             // CONTINUE 
    }
    
    // no need to be extrapolated? 
    if ( std::fabs ( dau->referencePoint().Z() - refPoint.Z() ) <= m_dz ) 
    {
      sum += LoKi::Kinematics::wrongMass ( dau->momentum() , newmass ) ;
      continue ;  // CONTINUE 
    }
    
    // extrapolate it! 
    static LHCb::Particle s_particle ;
    //
    const StatusCode ssc = m_transporter->transport ( dau , refPoint.Z() , s_particle ) ;
    if ( ssc.isSuccess() ) 
    { 
      sum += LoKi::Kinematics::wrongMass ( s_particle.momentum() , newmass ) ; 
    }
    else 
    {
      Warning ( "Unable to transport daughter particle, use non-extrapolated" , sc ) ;
      sum += LoKi::Kinematics::wrongMass ( dau->momentum() , newmass ) ;
    }
    
  } // end of the exlicit loop over daughter particles  
  
  return sum.M() ;
}
// ============================================================================
// OPTIONAL  : the nice printout 
// ============================================================================
std::ostream& LoKi::Particles::WrongMass::fillStream ( std::ostream& s ) const 
{
  if      ( !m_names.empty () ) 
  {
    if ( 4 == m_names.size() ) 
    {
      return 
        s << "WMASS(" 
          << Gaudi::Utils::toString ( m_names[0] ) << ","
          << Gaudi::Utils::toString ( m_names[1] ) << ","
          << Gaudi::Utils::toString ( m_names[2] ) << ","
          << Gaudi::Utils::toString ( m_names[3] ) << ")" ;
    }  
    else if ( 3 == m_names.size() ) 
    {
      return 
        s << "WMASS(" 
          << Gaudi::Utils::toString ( m_names[0] ) << ","
          << Gaudi::Utils::toString ( m_names[1] ) << ","
          << Gaudi::Utils::toString ( m_names[2] ) << ")" ;
    } 
    else if ( 2 == m_names.size() ) 
    {
      return 
        s << "WMASS(" 
          << Gaudi::Utils::toString ( m_names[0] ) << ","
          << Gaudi::Utils::toString ( m_names[1] ) << ")" ;
    }     
    else if ( 1 == m_names.size() ) 
    {
      return 
        s << "WMASS(" 
          << Gaudi::Utils::toString ( m_names[0] ) << ")" ;
    }
    return s << "WMASS(" << Gaudi::Utils::toString ( m_names ) << ")" ; 
  }
  else if ( !m_pids.empty  () )
  {
    s << "WMASS([" ;
    typedef std::vector<LHCb::ParticleID> PIDs ;
    for ( PIDs::const_iterator ipid = m_pids.begin() ; 
          m_pids.end () != ipid ; ++ipid )
    {
      if ( m_pids.begin() != ipid ) { s << "," ; }
      s << "LHCb.ParticleID( " << ipid->pid() << ")" ;
    }
    return s << "])" ;                                              // RETURN 
  }
  return s << "WMASS(" << Gaudi::Utils::toString ( m_masses ) << ")" ; 
}
// ======================================================================

// ======================================================================
// constructor from masse
// ======================================================================
LoKi::Particles::DeltaWrongMass::DeltaWrongMass
( const double            m0 , 
  const LoKi::Particles::WrongMass& wm ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , wm ) ) 
  , LoKi::Particles::WrongMass ( wm ) 
  , m_m0    ( m0 ) 
  , m_name0 (    ) 
  , m_pid0  ( 0  ) 
{}
// ======================================================================
// constructor from name 
// ======================================================================
LoKi::Particles::DeltaWrongMass::DeltaWrongMass
( const std::string&                m0 , 
  const LoKi::Particles::WrongMass& wm ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , wm ) ) 
  , LoKi::Particles::WrongMass ( wm ) 
  , m_m0    ( -100 * Gaudi::Units::TeV ) 
  , m_name0 ( m0 ) 
  , m_pid0  ( 0  ) 
{
  m_m0 = LoKi::Particles::massFromName ( m_name0 ) ;
}
// ======================================================================
// constructor from pid 
// ======================================================================
LoKi::Particles::DeltaWrongMass::DeltaWrongMass
( const LHCb::ParticleID&           m0 , 
  const LoKi::Particles::WrongMass& wm ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , wm ) ) 
  , LoKi::Particles::WrongMass ( wm ) 
  , m_m0    ( -100 * Gaudi::Units::TeV ) 
  , m_name0 (    ) 
  , m_pid0  ( m0 ) 
{
  m_m0 = LoKi::Particles::massFromPID ( m_pid0 ) ;
}
// ============================================================================
// evaluate the wrong mass 
// ============================================================================
LoKi::Particles::DeltaWrongMass::result_type 
LoKi::Particles::DeltaWrongMass::dwmass  
( LoKi::Particles::DeltaWrongMass::argument p ) const
{
  StatusCode sc = check ( p ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from check(), return InvalidMass" , sc ) ;
    return LoKi::Constants::InvalidMass ;
  }
  return dwmass ( p->daughters () ) ;
}
// ============================================================================
// OPTIONAL  : the nice printout 
// ============================================================================
std::ostream& LoKi::Particles::DeltaWrongMass::fillStream
( std::ostream& s ) const 
{
  s << "DWMASS(" ;
  //
  if      ( !m_name0.empty() )  { s << "'" << m_name0 << "'" ; }
  else if ( 0 != m_pid0.pid() ) { s <<        m_pid0         ; }
  else                          { s <<        m_m0           ; }
  //
  s << "," ;
  LoKi::Particles::WrongMass::fillStream( s ) ;
  //
  return s << ")";
}



// ======================================================================
// constructor from masse
// ======================================================================
LoKi::Particles::AbsDeltaWrongMass::AbsDeltaWrongMass
( const double            m0 , 
  const LoKi::Particles::WrongMass& wm ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , wm ) ) 
  , LoKi::Particles::DeltaWrongMass ( m0 , wm ) 
{}
// ======================================================================
// constructor from name 
// ======================================================================
LoKi::Particles::AbsDeltaWrongMass::AbsDeltaWrongMass
( const std::string&                m0 , 
  const LoKi::Particles::WrongMass& wm ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , wm ) ) 
  , LoKi::Particles::DeltaWrongMass ( m0 , wm ) 
{}
// ======================================================================
// constructor from pid 
// ======================================================================
LoKi::Particles::AbsDeltaWrongMass::AbsDeltaWrongMass
( const LHCb::ParticleID&           m0 , 
  const LoKi::Particles::WrongMass& wm ) 
  : LoKi::AuxFunBase ( std::tie ( m0 , wm ) ) 
  , LoKi::Particles::DeltaWrongMass ( m0 , wm ) 
{}
// ======================================================================
// constructor from delta 
// ======================================================================
LoKi::Particles::AbsDeltaWrongMass::AbsDeltaWrongMass
( const LoKi::Particles::DeltaWrongMass& dwm ) 
  : LoKi::Particles::DeltaWrongMass ( dwm ) 
{}
// ============================================================================
// evaluate the wrong mass 
// ============================================================================
LoKi::Particles::AbsDeltaWrongMass::result_type 
LoKi::Particles::AbsDeltaWrongMass::adwmass
( LoKi::Particles::AbsDeltaWrongMass::argument p ) const
{
  StatusCode sc = check ( p ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from check(), return InvalidMass" , sc ) ;
    return LoKi::Constants::InvalidMass ;
  }
  return adwmass ( p->daughters () ) ;
}
// ============================================================================
// OPTIONAL  : the nice printout 
// ============================================================================
std::ostream& LoKi::Particles::AbsDeltaWrongMass::fillStream
( std::ostream& s ) const 
{
  s << "ADWMASS(" ;
  //
  if      ( !name0().empty()  ) { s << "'" << name0 () << "'" ; }
  else if ( 0 != pid0().pid() ) { s <<        pid0  ()        ; }
  else                          { s <<        m0    ()        ; }
  //
  s << "," ;
  //
  LoKi::Particles::WrongMass::fillStream( s ) ;
  //
  return s << ")";
}
// ============================================================================
// The END 
// ============================================================================
