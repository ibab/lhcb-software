// $Id: Particles17.cpp,v 1.4 2007-07-23 17:35:54 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Particles0.h"
#include "LoKi/Particles17.h"
// ============================================================================
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-03-01
 */
// ============================================================================
// constructor from the angle
// ============================================================================
LoKi::Particles::DeltaPhi::DeltaPhi ( const double phi ) 
  : LoKi::Function<const LHCb::Particle*> () 
  , m_eval (     )
  , m_phi  ( phi )
{ 
  m_phi = adjust ( m_phi ) ; 
}  
// ============================================================================
// constructor from the vector 
// ============================================================================
LoKi::Particles::DeltaPhi::DeltaPhi ( const LoKi::ThreeVector& v ) 
  : LoKi::Function<const LHCb::Particle*> () 
  , m_eval (         )
  , m_phi  ( v.Phi() )
{ 
  m_phi = adjust ( m_phi ) ; 
}  
// ============================================================================
// constructor from the vector 
// ============================================================================
LoKi::Particles::DeltaPhi::DeltaPhi ( const LoKi::LorentzVector& v ) 
  : LoKi::Function<const LHCb::Particle*> () 
  , m_eval (         )
  , m_phi  ( v.Phi() )
{ 
  m_phi = adjust ( m_phi ) ; 
}  
// ============================================================================
// constructor from the particle
// ============================================================================
LoKi::Particles::DeltaPhi::DeltaPhi ( const LHCb::Particle* p )
  : LoKi::Function<const LHCb::Particle*> () 
  , m_eval ( )
  , m_phi  ( )
{ 
  if ( 0 == p ) { Exception("Invalid LHCb::Particle*") ;}
  m_phi = p->momentum().phi() ;
  m_phi = adjust ( m_phi ) ; 
}  
// ============================================================================
// copy constructor
// ============================================================================
LoKi::Particles::DeltaPhi::DeltaPhi 
( const LoKi::Particles::DeltaPhi& right ) 
  : LoKi::AuxFunBase                          ( right ) 
  , LoKi::Function<const LHCb::Particle*> ( right ) 
  , m_eval ( right.m_eval )
  , m_phi  ( right.m_phi  )
{ 
  m_phi = adjust ( m_phi ) ; 
}  
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::DeltaPhi*
LoKi::Particles::DeltaPhi::clone() const
{ return new LoKi::Particles::DeltaPhi(*this) ; }
// ============================================================================
// MANDATORY: the only essential method 
// ============================================================================
LoKi::Particles::DeltaPhi::result_type
LoKi::Particles::DeltaPhi::operator() 
  ( LoKi::Particles::DeltaPhi::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ("LHCb::Particle* points to NULL, return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  return adjust ( m_eval( p ) - m_phi ) ;
} 
// ============================================================================
// OPTIONAL: "SHORT" representation
// ============================================================================
std::ostream& LoKi::Particles::DeltaPhi::fillStream ( std::ostream& s ) const 
{ return s << "DPHI(" << m_phi << ")" ; }
// ============================================================================
// adjust delta phi into the range of [-180:180]degrees 
// ============================================================================
const double LoKi::Particles::DeltaPhi::adjust ( double phi ) const 
{
  static const double s_180 = 180 * Gaudi::Units::degree ;
  static const double s_360 = 360 * Gaudi::Units::degree ;
  //
  while ( phi >      s_180 ) { phi -= s_360 ; }
  while ( phi < -1 * s_180 ) { phi += s_360 ; }
  //
  return phi ; 
} 
// ============================================================================
// constructor from the angle
// ============================================================================
LoKi::Particles::DeltaEta::DeltaEta ( const double eta ) 
  : LoKi::Function<const LHCb::Particle*> () 
  , m_eval (     )
  , m_eta  ( eta )
{}
// ============================================================================
// constructor from the vector 
// ============================================================================
LoKi::Particles::DeltaEta::DeltaEta ( const LoKi::ThreeVector& v ) 
  : LoKi::Function<const LHCb::Particle*> () 
  , m_eval (         )
  , m_eta  ( v.Eta() )
{}
// ============================================================================
// constructor from the vector 
// ============================================================================
LoKi::Particles::DeltaEta::DeltaEta ( const LoKi::LorentzVector& v ) 
  : LoKi::Function<const LHCb::Particle*> () 
  , m_eval (         )
  , m_eta  ( v.Eta() )
{}
// ============================================================================
// constructor from the particle
// ============================================================================
LoKi::Particles::DeltaEta::DeltaEta ( const LHCb::Particle* p ) 
  : LoKi::Function<const LHCb::Particle*> () 
  , m_eval ( )
  , m_eta  ( )
{
  if ( 0 == p ) { Exception("Invalid LHCb::Particle*") ;}
  m_eta = p->momentum().Eta() ;
} 
// ============================================================================
// copy constructor
// ============================================================================
LoKi::Particles::DeltaEta::DeltaEta 
( const LoKi::Particles::DeltaEta& right ) 
  : LoKi::AuxFunBase                          ( right ) 
  , LoKi::Function<const LHCb::Particle*> ( right ) 
  , m_eval ( right.m_eval )
  , m_eta  ( right.m_eta  )
{}  
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::DeltaEta* 
LoKi::Particles::DeltaEta::clone() const 
{ return new LoKi::Particles::DeltaEta(*this) ; }
// ============================================================================
// MANDATORY: the only essential method 
// ============================================================================
LoKi::Particles::DeltaEta::result_type
LoKi::Particles::DeltaEta::operator() 
  ( LoKi::Particles::DeltaEta::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ("LHCb::Particle* points to NULL, return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  return m_eval( p ) - m_eta ;
} 
// ============================================================================
// OPTIONAL: "SHORT" representation
// ============================================================================
std::ostream& LoKi::Particles::DeltaEta::fillStream ( std::ostream& s ) const 
{ return s << "GDETA(" << m_eta << ")" ; }

// ============================================================================
// constructor from the eta, phi
// ============================================================================
LoKi::Particles::DeltaR2::DeltaR2 ( double eta , double phi ) 
  : LoKi::Function<const LHCb::Particle*> () 
  , m_dphi ( phi )
  , m_deta ( eta )
{} 
// ============================================================================
// constructor from the vector 
// ============================================================================
LoKi::Particles::DeltaR2::DeltaR2 ( const LoKi::ThreeVector& v ) 
  : LoKi::Function<const LHCb::Particle*> () 
  , m_dphi ( v ) 
  , m_deta ( v ) 
{}
// ============================================================================
// constructor from the vector 
// ============================================================================
LoKi::Particles::DeltaR2::DeltaR2 ( const LoKi::LorentzVector& v ) 
  : LoKi::Function<const LHCb::Particle*> () 
  , m_dphi ( v ) 
  , m_deta ( v ) 
{}
// ============================================================================
// constructor from the particle
// ============================================================================
LoKi::Particles::DeltaR2::DeltaR2 ( const LHCb::Particle* v ) 
  : LoKi::Function<const LHCb::Particle*> () 
  , m_dphi ( v ) 
  , m_deta ( v ) 
{}
// ============================================================================
// copy constructor
// ============================================================================
LoKi::Particles::DeltaR2::DeltaR2 
( const LoKi::Particles::DeltaR2& right ) 
  : LoKi::AuxFunBase                          ( right ) 
  , LoKi::Function<const LHCb::Particle*> ( right ) 
  , m_dphi ( right.m_dphi )
  , m_deta ( right.m_deta )
{}  
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::DeltaR2*
LoKi::Particles::DeltaR2::clone() const 
{ return new LoKi::Particles::DeltaR2(*this) ; }
// ============================================================================
// MANDATORY: the only essential method 
// ============================================================================
LoKi::Particles::DeltaR2::result_type
LoKi::Particles::DeltaR2::operator() 
  ( LoKi::Particles::DeltaR2::argument p ) const 
{
  const double dphi = m_dphi ( p ) ;
  const double deta = m_deta ( p ) ;
  //
  return dphi*dphi + deta*deta ;
} ;
// ============================================================================
// OPTIONAL: "SHORT" representation
// ============================================================================
std::ostream& LoKi::Particles::DeltaR2::fillStream ( std::ostream& s ) const 
{ return s << "DR2(" << m_deta.eta() << "," << m_dphi.phi() << ")" ; }
// ============================================================================



// ============================================================================
// The END 
// ============================================================================

