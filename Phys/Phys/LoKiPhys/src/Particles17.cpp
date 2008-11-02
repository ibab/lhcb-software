// $Id: Particles17.cpp,v 1.8 2008-11-02 20:13:32 ibelyaev Exp $
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
  : LoKi::Particles::Phi () 
  , m_phi  ( phi )
{ 
  m_phi = adjust ( m_phi ) ; 
}  
// ============================================================================
// constructor from the vector 
// ============================================================================
LoKi::Particles::DeltaPhi::DeltaPhi ( const LoKi::ThreeVector& v ) 
  : LoKi::Particles::Phi ()
  , m_phi  ( v.Phi() )
{ 
  m_phi = adjust ( m_phi ) ; 
}  
// ============================================================================
// constructor from the vector 
// ============================================================================
LoKi::Particles::DeltaPhi::DeltaPhi ( const LoKi::LorentzVector& v ) 
  : LoKi::Particles::Phi ()
  , m_phi  ( v.Phi() )
{ 
  m_phi = adjust ( m_phi ) ; 
}  
// ============================================================================
// constructor from the particle
// ============================================================================
LoKi::Particles::DeltaPhi::DeltaPhi ( const LHCb::Particle* p )
  : LoKi::Particles::Phi ()
  , m_phi  ( )
{ 
  if ( 0 == p ) { Exception("Invalid LHCb::Particle*") ;}
  m_phi = p->momentum().phi() ;
  m_phi = adjust ( m_phi ) ; 
}  
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
  return dphi ( p ) ;
} 
// ============================================================================
// OPTIONAL: "SHORT" representation
// ============================================================================
std::ostream& LoKi::Particles::DeltaPhi::fillStream ( std::ostream& s ) const 
{ return s << "DPHI(" << m_phi << ")" ; }
// ============================================================================
// constructor from the angle
// ============================================================================
LoKi::Particles::DeltaEta::DeltaEta ( const double eta ) 
  : LoKi::Particles::PseudoRapidity () 
  , m_eta  ( eta )
{}
// ============================================================================
// constructor from the vector 
// ============================================================================
LoKi::Particles::DeltaEta::DeltaEta ( const LoKi::ThreeVector& v ) 
  : LoKi::Particles::PseudoRapidity () 
  , m_eta  ( v.Eta() )
{}
// ============================================================================
// constructor from the vector 
// ============================================================================
LoKi::Particles::DeltaEta::DeltaEta ( const LoKi::LorentzVector& v ) 
  : LoKi::Particles::PseudoRapidity () 
  , m_eta  ( v.Eta() )
{}
// ============================================================================
// constructor from the particle
// ============================================================================
LoKi::Particles::DeltaEta::DeltaEta ( const LHCb::Particle* p ) 
  : LoKi::Particles::PseudoRapidity () 
  , m_eta  ( )
{
  if ( 0 == p ) { Exception("Invalid LHCb::Particle*") ;}
  m_eta = p->momentum().Eta() ;
} 
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
  return deta ( p ) ;
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
  : LoKi::Particles::DeltaPhi ( phi )  
  , m_deta ( eta )
{} 
// ============================================================================
// constructor from the vector 
// ============================================================================
LoKi::Particles::DeltaR2::DeltaR2 ( const LoKi::ThreeVector& v ) 
  : LoKi::Particles::DeltaPhi ( v )  
  , m_deta ( v ) 
{}
// ============================================================================
// constructor from the vector 
// ============================================================================
LoKi::Particles::DeltaR2::DeltaR2 ( const LoKi::LorentzVector& v ) 
  : LoKi::Particles::DeltaPhi ( v )  
  , m_deta ( v ) 
{}
// ============================================================================
// constructor from the particle
// ============================================================================
LoKi::Particles::DeltaR2::DeltaR2 ( const LHCb::Particle* v ) 
  : LoKi::Particles::DeltaPhi ( v )  
  , m_deta ( v ) 
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
  if ( 0 == p ) 
  {
    Error ("LHCb::Particle* points to NULL, return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  //
  const double dphi_ = dphi ( p ) ;
  const double deta_ = deta ( p ) ;
  //
  return dphi_*dphi_ + deta_*deta_ ;
} ;
// ============================================================================
// OPTIONAL: "SHORT" representation
// ============================================================================
std::ostream& LoKi::Particles::DeltaR2::fillStream ( std::ostream& s ) const 
{ return s << "DR2(" << eta0() << "," << phi0() << ")" ; }
// ============================================================================



// ============================================================================
// The END 
// ============================================================================

