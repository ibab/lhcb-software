// $Id: Particles23.cpp,v 1.1 2008-10-13 16:32:24 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
#include "GaudiKernel/ParticleProperty.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Objects.h"
#include "LoKi/Particles23.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/WrongMass.h"
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
  const double m2 ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  () 
{
  m_masses.push_back ( m1 ) ; 
  m_masses.push_back ( m2 ) ;
}
// ============================================================================
LoKi::Particles::WrongMass::WrongMass
( const double m1 ,
  const double m2 ,
  const double m3 ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  () 
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
  const double m4 ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  () 
{
  m_masses.push_back ( m1 ) ; 
  m_masses.push_back ( m2 ) ;
  m_masses.push_back ( m3 ) ;
  m_masses.push_back ( m4 ) ;
}
// ============================================================================
LoKi::Particles::WrongMass::WrongMass
( const std::vector<double>& masses ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses ( masses ) 
  , m_pids   () 
  , m_names  () 
{}
// ============================================================================
LoKi::Particles::WrongMass::WrongMass 
( const LHCb::ParticleID& p1 , 
  const LHCb::ParticleID& p2 ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  () 
{
  m_pids.push_back ( p1 ) ;
  m_pids.push_back ( p2 ) ;  
  decode().ignore () ;
}
// ============================================================================
LoKi::Particles::WrongMass::WrongMass 
( const LHCb::ParticleID& p1 , 
  const LHCb::ParticleID& p2 ,
  const LHCb::ParticleID& p3 ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  () 
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
  const LHCb::ParticleID& p4 ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  () 
{
  m_pids.push_back ( p1 ) ;
  m_pids.push_back ( p2 ) ;  
  m_pids.push_back ( p3 ) ;
  m_pids.push_back ( p4 ) ;  
  decode().ignore () ;
}
// ============================================================================
LoKi::Particles::WrongMass::WrongMass 
( const std::vector<LHCb::ParticleID>& pids ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   ( pids ) 
  , m_names  () 
{
  decode().ignore () ;
}
// ============================================================================
LoKi::Particles::WrongMass::WrongMass 
( const std::string& p1 , 
  const std::string& p2 ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  () 
{
  m_names.push_back ( p1 ) ;
  m_names.push_back ( p2 ) ;  
  decode().ignore () ;
}
// ============================================================================
LoKi::Particles::WrongMass::WrongMass 
( const std::string& p1 , 
  const std::string& p2 ,
  const std::string& p3 ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  () 
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
  const std::string& p4 ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  () 
{
  m_names.push_back ( p1 ) ;
  m_names.push_back ( p2 ) ;  
  m_names.push_back ( p3 ) ;  
  m_names.push_back ( p4 ) ;  
  decode().ignore () ;
}
// ============================================================================
LoKi::Particles::WrongMass::WrongMass 
( const std::vector<std::string>& names )  
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_masses () 
  , m_pids   () 
  , m_names  ( names ) 
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
      const ParticleProperty* pp = LoKi::Particles::_ppFromPID ( *ipid ) ;
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
      const ParticleProperty* pp = LoKi::Particles::_ppFromName ( *iname ) ;
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
// MANDATORY : the only one essential method 
// ============================================================================
LoKi::Particles::WrongMass::result_type 
LoKi::Particles::WrongMass::operator() 
  ( LoKi::Particles::WrongMass::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ( "Invalid LHCb::Particle*, return Invalid Mass" ) ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  if ( m_masses.empty() ) 
  {
    StatusCode sc = decode() ;
    if ( sc.isFailure() ) 
    {
      Error ( "Invalid configuration, return Invalid Mass" , sc ) ;
      return LoKi::Constants::InvalidMass ;      
    }
  }
  //
  typedef  SmartRefVector<LHCb::Particle> Daughters ;
  const Daughters& daughters = p->daughters() ;
  if ( m_masses.size() != daughters.size() ) 
  {
    Error ( " #masses != #daughetrs, return Invalid Mass" ) ;
    return LoKi::Constants::InvalidMass ; 
  }
  // evaluate the 'wrong mass'
  const LoKi::LorentzVector wm = 
    LoKi::Kinematics::wrongMass 
    ( daughters.begin () , 
      daughters.end   () , 
      m_masses.begin  () , LoKi::Objects::_VALID_ ) ;
  // 
  return wm.M() ;
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
// ============================================================================
// The END 
// ============================================================================
